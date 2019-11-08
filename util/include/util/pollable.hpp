#ifndef UTIL_POLLABLE_HPP
#define UTIL_POLLABLE_HPP

#include <mutex>
#include <type_traits>

/**
 * Wrapper class around a value that can be updated from elsewhere and be polled for
 * whether such updates have happened.
 */
template <typename T, bool atomic = false>
class _Pollable {
    T value;
    bool dirty = false;
    std::enable_if<atomic, std::mutex> mutex;

  public:
    _Pollable() = default;
    _Pollable(T &&value) : value(value), dirty(false) {}

    bool is_dirty() const { return dirty; }

    void reset(T &&value)
    {
        if constexpr (atomic) {
            std::scoped_lock lock{mutex};
        }
        dirty = true;
        this->value = value;
    }

    void reset(const T &value)
    {
        if constexpr (atomic) {
            std::scoped_lock lock{mutex};
        }
        dirty = true;
        this->value = value;
    }

    T get()
    {
        if constexpr (atomic) {
            std::scoped_lock lock{mutex};
        }
        dirty = false;
        return value;
    }

    _Pollable<T> &operator=(const T &t)
    {
        reset(t);
        return *this;
    }

    T operator*() { return get(); }
    operator bool() const { return dirty; }
};

// Deduction guide
template <typename T>
_Pollable(T)->_Pollable<T>;

template <typename T>
using AtomicPollable = _Pollable<T, true>;
template <typename T>
using Pollable = _Pollable<T, false>;


#endif
