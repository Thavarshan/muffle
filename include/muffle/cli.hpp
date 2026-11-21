/**
 * @file cli.hpp
 * @brief Command-line argument parsing for the Muffle executable.
 */
#ifndef MUFFLE_CLI_HPP
#define MUFFLE_CLI_HPP

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace muffle {

/** @brief Requested action after argument parsing. */
enum class Action : std::uint8_t {
    run,     /**< Execute the provided command. */
    help,    /**< Display command-line usage. */
    version, /**< Display the current version. */
    error    /**< Report invalid arguments. */
};

/** @brief Parsed options passed into the command runner. */
struct Options {
    /** @brief Maximum number of captured lines shown on failure. */
    std::size_t tail_lines{40};
    /** @brief Print command output as it is received. */
    bool verbose{false};
    /** @brief Optional path for a persistent log. */
    std::optional<std::string> log_path;
    /** @brief Command and arguments, never interpreted by a shell. */
    std::vector<std::string> command;
};

/** @brief The outcome of parsing command-line arguments. */
struct ParseResult {
    /** @brief Action the executable should perform. */
    Action action{Action::error};
    /** @brief Options for the run action. */
    Options options;
    /** @brief Explanation when parsing fails. */
    std::string error_message;
};

/**
 * @brief Parse options without executing the user's command.
 * @param arguments Command-line tokens, excluding the program name.
 * @return An action and validated options, or an error message.
 */
ParseResult parse_arguments(const std::vector<std::string>& arguments);

/**
 * @brief Return the public command-line usage text.
 * @return Help text ready to print.
 */
std::string usage();

} // namespace muffle

#endif // MUFFLE_CLI_HPP
