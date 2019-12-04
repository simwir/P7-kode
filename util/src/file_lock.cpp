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
