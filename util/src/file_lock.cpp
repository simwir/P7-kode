#include "util/file_lock.hpp"

#include <cassert>
#include <cstdio>
#include <fstream>
#include <sys/file.h>
#include <unistd.h>

bool is_locked(std::filesystem::path path)
{
    return std::filesystem::exists(path += _suffix);
}

FileLock::FileLock(const std::filesystem::path &path, lock_mode mode) : held_path(path)
{
    held_path += _suffix;
    fd = open(held_path.c_str(), O_CREAT | O_RDONLY, S_IRWXU);
    if (fd == -1) {
        throw CouldNotLockFile{"Error opening file. Errno: " + std::to_string(errno)};
    }
    if (flock(fd, mode == lock_mode::exclusive ? LOCK_EX : LOCK_SH) != 0) {
        throw CouldNotLockFile{"Could not lock file. Errno: " + std::to_string(errno) +
                               " fd: " + std::to_string(fd)};
    }
}

FileLock::~FileLock()
{
    close(fd);
}
