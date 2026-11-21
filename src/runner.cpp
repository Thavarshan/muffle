#include "muffle/runner.hpp"

#include <algorithm>
#include <cerrno>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <poll.h>
#include <string>
#include <vector>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

namespace muffle {
namespace {

constexpr int internal_error = 125;
constexpr std::size_t block_size = 8192;
volatile std::sig_atomic_t child_group = 0;
volatile std::sig_atomic_t received_signal = 0;

class File {
  public:
    explicit File(int fd = -1) : fd_(fd) {
    }
    ~File() {
        close();
    }
    File(const File&) = delete;
    File& operator=(const File&) = delete;

    int get() const {
        return fd_;
    }
    void close() {
        if (fd_ >= 0) {
            ::close(fd_);
            fd_ = -1;
        }
    }

  private:
    int fd_;
};

void forward_signal(int signal) {
    received_signal = signal;
    const auto group = child_group;
    if (group > 0) {
        if (::kill(-group, signal) != 0) {
            ::kill(group, signal);
        }
    }
}

class SignalGuard {
  public:
    SignalGuard() {
        struct sigaction action{};
        action.sa_handler = forward_signal;
        sigemptyset(&action.sa_mask);
        action.sa_flags = 0;
        installed_int_ = ::sigaction(SIGINT, &action, &old_int_) == 0;
        installed_term_ = ::sigaction(SIGTERM, &action, &old_term_) == 0;
        action.sa_handler = SIG_IGN;
        installed_pipe_ = ::sigaction(SIGPIPE, &action, &old_pipe_) == 0;
    }

    ~SignalGuard() {
        if (installed_int_) {
            ::sigaction(SIGINT, &old_int_, nullptr);
        }
        if (installed_term_) {
            ::sigaction(SIGTERM, &old_term_, nullptr);
        }
        if (installed_pipe_) {
            ::sigaction(SIGPIPE, &old_pipe_, nullptr);
        }
        child_group = 0;
        received_signal = 0;
    }

    SignalGuard(const SignalGuard&) = delete;
    SignalGuard& operator=(const SignalGuard&) = delete;

  private:
    struct sigaction old_int_{};
    struct sigaction old_term_{};
    struct sigaction old_pipe_{};
    bool installed_pipe_{false};
    bool installed_int_{false};
    bool installed_term_{false};
};

bool write_all(int fd, const char* data, std::size_t length) {
    std::size_t written = 0;
    while (written < length) {
        const auto count = ::write(fd, data + written, length - written);
        if (count < 0 && errno == EINTR) {
            continue;
        }
        if (count <= 0) {
            return false;
        }
        written += static_cast<std::size_t>(count);
    }
    return true;
}

bool read_at(int fd, char* data, std::size_t length, off_t offset) {
    std::size_t received = 0;
    while (received < length) {
        const auto count = ::pread(fd, data + received, length - received,
                                   offset + static_cast<off_t>(received));
        if (count < 0 && errno == EINTR) {
            continue;
        }
        if (count <= 0) {
            return false;
        }
        received += static_cast<std::size_t>(count);
    }
    return true;
}

struct CaptureFiles {
    const File& source;
    const File& log;
};

bool read_and_capture(CaptureFiles files, bool verbose) {
    char buffer[block_size];
    bool output_usable = true;
    while (true) {
        if (received_signal != 0) {
            return true;
        }
        struct pollfd descriptor{};
        descriptor.fd = files.source.get();
        descriptor.events = POLLIN | POLLHUP;
        const int ready = ::poll(&descriptor, 1, 100);
        if (ready < 0) {
            if (errno == EINTR) {
                continue;
            }
            return false;
        }
        if (ready == 0) {
            continue;
        }
        const auto count = ::read(files.source.get(), buffer, sizeof(buffer));
        if (count == 0) {
            return true;
        }
        if (count < 0) {
            if (errno == EINTR) {
                continue;
            }
            return false;
        }
        const auto size = static_cast<std::size_t>(count);
        if (!write_all(files.log.get(), buffer, size)) {
            return false;
        }
        if (verbose && output_usable) {
            output_usable = write_all(STDOUT_FILENO, buffer, size);
        }
    }
}

bool print_tail(const File& log, std::size_t lines) {
    const int fd = log.get();
    const auto size = ::lseek(fd, 0, SEEK_END);
    if (size < 0) {
        return false;
    }
    off_t position = size;
    off_t first = 0;
    std::size_t found = 0;
    bool at_end = true;
    char buffer[block_size];
    while (position > 0) {
        const auto chunk =
            std::min(block_size, static_cast<std::size_t>(position));
        position -= static_cast<off_t>(chunk);
        if (!read_at(fd, buffer, chunk, position)) {
            return false;
        }
        for (std::size_t index = chunk; index > 0; --index) {
            const char ch = buffer[index - 1];
            if (at_end) {
                at_end = false;
                if (ch == '\n') {
                    continue;
                }
            }
            if (ch == '\n' && ++found == lines) {
                first = position + static_cast<off_t>(index);
                position = 0;
                break;
            }
        }
    }
    position = first;
    while (position < size) {
        const auto remaining = size - position;
        const auto chunk =
            std::min(block_size, static_cast<std::size_t>(remaining));
        if (!read_at(fd, buffer, chunk, position)) {
            return false;
        }
        if (!write_all(STDERR_FILENO, buffer, chunk)) {
            return false;
        }
        position += static_cast<off_t>(chunk);
    }
    if (size > first && read_at(fd, buffer, 1, size - 1) && buffer[0] != '\n') {
        std::cerr << '\n';
    }
    return true;
}

bool stop_child(pid_t child, int signal, int& status);

bool wait_child(pid_t child, int& status) {
    while (true) {
        const auto waited = ::waitpid(child, &status, 0);
        if (waited == child) {
            return true;
        }
        if (waited < 0 && errno == EINTR) {
            const int signal = received_signal;
            if (signal != 0) {
                return stop_child(child, signal, status);
            }
            continue;
        }
        return false;
    }
}

} // namespace

int translate_wait_status(int status) {
    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }
    if (WIFSIGNALED(status)) {
        return 128 + WTERMSIG(status);
    }
    return internal_error;
}

namespace {

bool stop_child(pid_t child, int signal, int& status) {
    if (::kill(-child, signal) != 0) {
        ::kill(child, signal);
    }
    const auto deadline =
        std::chrono::steady_clock::now() + std::chrono::milliseconds(500);
    while (std::chrono::steady_clock::now() < deadline) {
        const auto waited = ::waitpid(child, &status, WNOHANG);
        if (waited == child) {
            ::kill(-child, SIGKILL);
            return true;
        }
        if (waited < 0 && errno != EINTR) {
            return false;
        }
        ::poll(nullptr, 0, 20);
    }
    ::kill(-child, SIGKILL);
    ::kill(child, SIGKILL);
    while (::waitpid(child, &status, 0) < 0) {
        if (errno != EINTR) {
            return false;
        }
    }
    return true;
}

} // namespace

int run_command(const Options& options) {
    if (options.command.empty()) {
        std::cerr << "muffle: no command supplied\n";
        return 2;
    }

    std::string log_path;
    int log_fd = -1;
    const bool persistent = options.log_path.has_value();
    if (persistent) {
        log_path = *options.log_path;
        log_fd = ::open(log_path.c_str(), O_CREAT | O_EXCL | O_RDWR | O_CLOEXEC,
                        0600);
    } else {
        const char* const temp = std::getenv("TMPDIR");
        const std::string directory = temp != nullptr ? temp : "/tmp";
        std::string pattern = directory + "/muffle-XXXXXX";
        std::vector<char> pathname(pattern.begin(), pattern.end());
        pathname.push_back('\0');
        log_fd = ::mkstemp(pathname.data());
        if (log_fd >= 0) {
            log_path = pathname.data();
        }
    }
    if (log_fd < 0) {
        std::cerr << "muffle: cannot create log: " << std::strerror(errno)
                  << '\n';
        return internal_error;
    }
    File log(log_fd);

    int fds[2];
    if (::pipe(fds) != 0) {
        std::cerr << "muffle: pipe failed: " << std::strerror(errno) << '\n';
        if (!persistent) {
            ::unlink(log_path.c_str());
        }
        return internal_error;
    }
    File reader(fds[0]);
    File writer(fds[1]);

    auto owned_command = options.command;
    std::vector<char*> command;
    command.reserve(owned_command.size() + 1);
    for (auto& argument : owned_command) {
        command.push_back(argument.data());
    }
    command.push_back(nullptr);

    const auto started = std::chrono::steady_clock::now();
    SignalGuard signals;
    const auto child = ::fork();
    if (child < 0) {
        std::cerr << "muffle: fork failed: " << std::strerror(errno) << '\n';
        if (!persistent) {
            ::unlink(log_path.c_str());
        }
        return internal_error;
    }
    if (child == 0) {
        ::signal(SIGPIPE, SIG_DFL);
        ::setpgid(0, 0);
        reader.close();
        if (::dup2(writer.get(), STDOUT_FILENO) < 0 ||
            ::dup2(writer.get(), STDERR_FILENO) < 0) {
            _exit(125);
        }
        writer.close();
        log.close();
        ::execvp(command.front(), command.data());
        const int error = errno;
        ::dprintf(STDERR_FILENO, "muffle: exec failed: %s\n",
                  std::strerror(error));
        _exit(error == ENOENT ? 127 : 126);
    }

    child_group = child;
    ::setpgid(child, child);
    writer.close();
    bool captured = read_and_capture({reader, log}, options.verbose);
    const int interrupted_by = received_signal;
    reader.close();
    int child_status = 0;
    bool waited = false;
    if (captured && interrupted_by == 0) {
        waited = wait_child(child, child_status);
    } else if (interrupted_by != 0) {
        waited = stop_child(child, interrupted_by, child_status);
        if (!waited) {
            waited = wait_child(child, child_status);
        }
    } else {
        ::kill(-child, SIGKILL);
        ::kill(child, SIGKILL);
        waited = wait_child(child, child_status);
    }
    child_group = 0;

    if (!captured || !waited) {
        std::cerr << "muffle: capture or wait failed\n";
        std::cerr << "Log: " << log_path << '\n';
        std::cout.flush();
        std::cerr.flush();
        return internal_error;
    }

    const int final_signal = received_signal;
    const int status = final_signal != 0 ? 128 + final_signal
                                         : translate_wait_status(child_status);
    const auto elapsed = std::chrono::duration<double>(
        std::chrono::steady_clock::now() - started);
    if (status == 0) {
        std::cout << "muffle: passed (" << std::fixed << std::setprecision(2)
                  << elapsed.count() << "s)\n";
        if (!persistent) {
            log.close();
            ::unlink(log_path.c_str());
        }
    } else {
        std::cerr << "muffle: failed (exit " << status << ", " << std::fixed
                  << std::setprecision(2) << elapsed.count() << "s)\n";
        if (!print_tail(log, options.tail_lines)) {
            std::cerr << "muffle: unable to display log tail\n";
        }
        std::cerr << "Log: " << log_path << '\n';
    }
    std::cout.flush();
    std::cerr.flush();
    return status;
}

} // namespace muffle
