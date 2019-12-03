#ifndef UTIL_FILELOCK
#define UTIL_FILELOCK

#include <filesystem>

const std::filesystem::path _suffix = "#.lock";

enum class lock_mode { shared, exclusive };

bool is_locked(std::filesystem::path path);

struct CouldNotLockFile : std::runtime_error {
    CouldNotLockFile(const std::string &s) :std::runtime_error(s) {}
};

class FileLock {
  public:
    FileLock(const std::filesystem::path &path, lock_mode mode = lock_mode::exclusive);
    ~FileLock();

  private:
    int fd;
    std::filesystem::path held_path;
};

#endif
