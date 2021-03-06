/*Copyright 2019 Anders Madsen, Emil Jørgensen Njor, Emil Stenderup Bækdahl, Frederik Baymler
 *Mathiesen, Nikolaj Jensen Ulrik, Simon Mejlby Virenfeldt
 *
 *Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 *associated documentation files (the "Software"), to deal in the Software without restriction,
 *including without limitation the rights to use, copy, modify, merge, publish, distribute,
 *sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 *furnished to do so, subject to the following conditions:
 *
 *The above copyright notice and this permission notice shall be included in all copies or
 *substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
 *NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 *DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
 *OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
// POSIX includes
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// Other includes
#include <iostream>
#include <sstream>
#include <uppaal_executor.hpp>

constexpr int PARENT_READ = 0;
constexpr int CHILD_WRITE = 1;
constexpr int CHILD_READ = 2;
constexpr int PARENT_WRITE = 3;

constexpr int NO_FLAGS = 0;

void scheduling::UppaalExecutor::execute(std::function<void(const std::string &)> callback)
{
    pid_t pid;
    int fd[4];

    pipe(fd);
    pipe(fd + 2);

    pid = fork();

    if (pid == 0) {
        // Child
        dup2(fd[CHILD_WRITE], STDOUT_FILENO);
        dup2(fd[CHILD_READ], STDIN_FILENO);
        close(fd[PARENT_WRITE]);
        close(fd[PARENT_READ]);

        const char *command = "verifyta";

        int ret = execlp(command, command, model_path.c_str(), query_path.c_str(), nullptr);

        if (ret == -1) {
            throw SchedulingException("Could not start verifyta. errno: " + std::to_string(errno) +
                                      ".");
        }

        child_pid = std::nullopt;
    }
    else {
        child_pid = {pid};
        // Parent
        close(fd[CHILD_WRITE]);
        close(fd[CHILD_READ]);
        close(fd[PARENT_WRITE]);

        // Wait for completion
        std::cout << "Waiting for completion...\n";
        worker = std::thread([&, parent_read = fd[PARENT_READ], callback]() -> void {
            int status;
            int res = waitpid(pid, &status, NO_FLAGS);
            if (res == -1) {
                std::cerr << "An error " << errno << " occured while waiting for process " << pid
                          << std::endl;
            }
            child_pid = std::nullopt;
            std::cout << "Scheduling complete with status " << status << ".\n";

            if (WIFSIGNALED(status)) { // Is true if the pid was terminated by a signal
                std::cerr << "was signaled" << std::endl;
                child_pid = {};
                return;
            }

            // Only do something if we actually did get a result
            if (status != 0) {
                // Cleanup after use
                close(parent_read);

                throw SchedulingException{"Could not start verifyta."};
            }

            // Read all from pipe
            std::stringstream ss;
            char buffer[2048];
            ssize_t bytes = 0;

            while ((bytes = read(parent_read, buffer, 2048)) > 0) {
                ss.write(buffer, bytes);
            }
            if (errno == EAGAIN) {
                std::cerr << "ERROR: verifyta did not provide any input" << std::endl;
            }

            // Cleanup after use
            close(parent_read);

            callback(ss.str());
        });
    }
}

bool scheduling::UppaalExecutor::abort()
{
    if (child_pid) {
        if (kill(*child_pid, SIGTERM) == 0) {
            child_pid = std::nullopt;

            // force join the thread to reset thread state.
            wait_for_result();
            return true;
        }
        else {
            // ESRCH = process not found. Treat this as a success.
            if (0 && errno == ESRCH) {
                child_pid = std::nullopt;
                return true;
            }
            else {
                std::cerr << errno << std::endl;

                return false;
            }
        }
    }
    // success
    return true;
}
