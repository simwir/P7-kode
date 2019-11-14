// POSIX includes
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

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

    std::cerr << "forking... \n";
    pid = fork();
    std::cerr << "done!" << std::endl;

    if (pid == 0) {
        std::cerr << "Child!" << std::endl;
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
        std::cerr << "Parent to PID " << pid << std::endl;
        child_pid = {pid};
        // Parent
        close(fd[CHILD_WRITE]);
        close(fd[CHILD_READ]);
        close(fd[PARENT_WRITE]);

        // Wait for completion
        std::cout << "Waiting for completion...\n";
        worker = std::thread([&, parent_read=fd[PARENT_READ], callback]() -> void {
            int status;
            int res = waitpid(pid, &status, NO_FLAGS);
            if (res == -1) {
                std::cerr << errno << std::endl;
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

            /*int flags = fcntl(fd[PARENT_READ], F_GETFL, 0);
              fcntl(fd[PARENT_READ], F_SETFL, flags | O_NONBLOCK);*/
            while ((bytes = read(parent_read, buffer, 2048)) > 0) {
                //std::cerr << buffer << std::endl;
                ss.write(buffer, bytes);
            }
            if (errno == EAGAIN) {
                std::cerr << "ERROR: verifyta did not provide any input" << std::endl;
            }

            std::cout << ss.str() << std::endl;

            // Cleanup after use
            close(parent_read);

            std::cerr << "invoking callback" << std::endl;
            callback(ss.str());
            std::cerr << "callback done" << std::endl;
        });
    }
}

bool scheduling::UppaalExecutor::abort()
{
    if (child_pid) {
        std::cerr << "killing\n";
        if (kill(*child_pid, SIGTERM) == 0) {
            std::cerr << "successfully killed :D" << std::endl;
            child_pid = std::nullopt;

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
