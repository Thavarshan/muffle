/**
 * @file runner.hpp
 * @brief Native command execution and output capture.
 */
#ifndef MUFFLE_RUNNER_HPP
#define MUFFLE_RUNNER_HPP

#include "muffle/cli.hpp"

namespace muffle {

/**
 * @brief Execute a command with Muffle's capture policy.
 * @param options Validated options containing the requested command.
 * @return The child's exit code, or an internal failure code.
 */
int run_command(const Options& options);

/**
 * @brief Translate a wait status into the public child status convention.
 * @param status Status value returned by waitpid.
 * @return Child exit code, 128 plus terminating signal, or 125 internally.
 */
int translate_wait_status(int status);

} // namespace muffle

#endif // MUFFLE_RUNNER_HPP
