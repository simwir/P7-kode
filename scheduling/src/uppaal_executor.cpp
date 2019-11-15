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
    }
    else {
        reset_pid(pid);
        // Parent
        close(fd[CHILD_WRITE]);
        close(fd[CHILD_READ]);
        close(fd[PARENT_WRITE]);

        // Wait for completion
        std::cout << "Waiting for completion..." << std::endl;
        worker = std::thread([&, parent_read = fd[PARENT_READ], callback]() -> void {
            int status;
            int pid;

            std::unique_lock get_pid{pid_lock};
            if (!child_pid.has_value()) {
                return; // TODO better behaviour?
            }
            else {
                pid = child_pid.value();
            }
            get_pid.unlock();

            int res = waitpid(pid, &status, NO_FLAGS);
            if (res == -1) {
                std::cerr << "ERROR: waitpid failed with errno " << errno << std::endl;
                if (errno == ECHILD) {
                    std::cerr << "ERROR: ECHILD (no child processes) for PID " << pid << std::endl;
                }
            }
            reset_pid();
            std::cout << "Scheduling complete with status " << status << ".\n";

            if (WIFSIGNALED(status)) { // Is true if the pid was terminated by a signal
                std::cerr << "was signaled" << std::endl;
                return;
            }

            // Only do something if we actually did get a result
            if (!WIFEXITED(status) && !(WEXITSTATUS(status) == EXIT_SUCCESS)) {
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
    std::scoped_lock _{pid_lock};
    if (child_pid) {
        if (kill(*child_pid, SIGTERM) == 0) {
            child_pid = std::nullopt;

            // force join the thread to reset thread state.
            wait_for_result();
            return true;
        }
        else {
            // ESRCH = process not found. Treat this as a success.
            if (errno == ESRCH) {
                child_pid = std::nullopt;
                return true;
            }
            else {
                std::cerr << "WARNING: could not abort. errno: " << errno << std::endl;

                return false;
            }
        }
    }
    // success
    return true;
}
