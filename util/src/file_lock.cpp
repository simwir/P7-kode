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
    fd = open(held_path.c_str(), O_CREAT | O_DIRECT);
    if (flock(fd, mode == lock_mode::exclusive ? LOCK_EX : LOCK_SH) != 0) {
        throw CouldNotLockFile{"Could not lock file. Errno: " + std::to_string(errno)};
    }

}

FileLock::~FileLock()
{
    close(fd);
}
