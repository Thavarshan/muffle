#include "muffle/cli.hpp"
#include "muffle/runner.hpp"

#include <iostream>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
    std::vector<std::string> arguments;
    for (int index = 1; index < argc; ++index) {
        arguments.emplace_back(argv[index]);
    }

    const auto parsed = muffle::parse_arguments(arguments);
    switch (parsed.action) {
    case muffle::Action::help:
        std::cout << muffle::usage();
        return 0;
    case muffle::Action::version:
        std::cout << "muffle " << MUFFLE_VERSION << '\n';
        return 0;
    case muffle::Action::error:
        std::cerr << "muffle: " << parsed.error_message << '\n';
        std::cerr << "Use --help for usage.\n";
        return 2;
    case muffle::Action::run:
        return muffle::run_command(parsed.options);
    }
    return 2;
}
