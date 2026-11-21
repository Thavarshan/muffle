#include "muffle/cli.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

void require(bool condition, const std::string& message) {
  if (!condition) {
    throw std::runtime_error(message);
  }
}

void test_command_tokens() {
  const auto result = muffle::parse_arguments(
      {"--", "printf", "%s", "hello world"});
  require(result.action == muffle::Action::run, "run action");
  require(result.options.command.size() == 3, "argument count");
  require(result.options.command.at(2) == "hello world",
          "arguments stay unmodified");
}

void test_options() {
  const auto result = muffle::parse_arguments(
      {"--verbose", "--tail", "99", "--log", "report.log",
       "--", "echo", "ok"});
  require(result.action == muffle::Action::run, "run action");
  require(result.options.verbose, "verbose flag");
  require(result.options.tail_lines == 99, "tail value");
  require(result.options.log_path == "report.log", "log path");
}

void test_errors() {
  const std::vector<std::vector<std::string>> cases = {
      {}, {"--"}, {"--tail"}, {"--tail", "0", "--", "x"},
      {"--tail", "abc", "--", "x"},
      {"--tail", "184467440737095516161", "--", "x"},
      {"--unknown"}, {"--log"}};
  for (const auto& arguments : cases) {
    const auto result = muffle::parse_arguments(arguments);
    require(result.action == muffle::Action::error,
            "invalid input rejected");
    require(!result.error_message.empty(), "error includes reason");
  }
}

void test_help_and_version() {
  require(muffle::parse_arguments({"--help"}).action ==
              muffle::Action::help,
          "help action");
  require(muffle::parse_arguments({"--version"}).action ==
              muffle::Action::version,
          "version action");
  require(muffle::usage().find("Usage: muffle") != std::string::npos,
          "usage text");
}

}  // namespace

int main() {
  try {
    test_command_tokens();
    test_options();
    test_errors();
    test_help_and_version();
    std::cout << "CLI unit tests passed\n";
  } catch (const std::exception& error) {
    std::cerr << "CLI unit test failed: " << error.what() << '\n';
    return 1;
  }
  return 0;
}
