#include "muffle/cli.hpp"

#include <charconv>
#include <iterator>
#include <string>
#include <system_error>
#include <vector>

namespace muffle {
namespace {

ParseResult failure(const std::string& reason) {
  ParseResult result;
  result.action = Action::error;
  result.error_message = reason;
  return result;
}

bool parse_tail(const std::string& token, std::size_t& value) {
  if (token.empty()) {
    return false;
  }
  const char* const start = token.data();
  const char* const end = start + token.size();
  const auto parsed = std::from_chars(start, end, value);
  return parsed.ec == std::errc{} && parsed.ptr == end && value > 0;
}

} // namespace

ParseResult parse_arguments(const std::vector<std::string>& arguments) {
  ParseResult result;
  for (std::size_t index = 0; index < arguments.size(); ++index) {
    const auto& arg = arguments[index];
    if (arg == "--") {
      result.options.command.assign(arguments.begin() +
                                        static_cast<std::ptrdiff_t>(index + 1),
                                    arguments.end());
      if (result.options.command.empty()) {
        return failure("expected a command after '--'");
      }
      result.action = Action::run;
      return result;
    }
    if (arg == "--help" || arg == "-h") {
      result.action = Action::help;
      return result;
    }
    if (arg == "--version") {
      result.action = Action::version;
      return result;
    }
    if (arg == "--verbose") {
      result.options.verbose = true;
      continue;
    }
    if (arg == "--tail" || arg == "--log") {
      if (++index >= arguments.size()) {
        return failure("missing value for " + arg);
      }
      if (arg == "--tail") {
        if (!parse_tail(arguments[index], result.options.tail_lines)) {
          return failure("--tail requires a positive integer");
        }
      } else if (arguments[index].empty()) {
        return failure("--log requires a non-empty path");
      } else {
        result.options.log_path = arguments[index];
      }
      continue;
    }
    return failure("unknown option: " + arg);
  }
  return failure("expected '--' followed by a command");
}

std::string usage() {
  return "Usage: muffle [--tail N] [--verbose] [--log PATH] -- "
         "COMMAND [ARG ...]\n"
         "       muffle --help | --version\n\n"
         "Options:\n"
         "  --tail N    Failure output length (default: 40).\n"
         "  --verbose   Stream captured output while running.\n"
         "  --log PATH  Request a persistent output log.\n"
         "  -h, --help  Show this help.\n"
         "  --version   Show the version.\n\n"
         "Logs are private; --log PATH must not already exist.\n";
}

} // namespace muffle
