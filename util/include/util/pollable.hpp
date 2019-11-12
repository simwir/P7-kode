#ifndef UTIL_POLLABLE_HPP
#define UTIL_POLLABLE_HPP

#include <mutex>
#include <type_traits>

/**
 * Wrapper class around a value that can be updated from elsewhere and be polled for
 * whether such updates have happened.
 */
template <typename T>
class Pollable {
    T value;
    bool dirty = false;
    mutable std::mutex mutex;

  public:
    Pollable() = default;
    Pollable(T &&value) : value(value), dirty(false) {}

    bool is_dirty() const { return dirty; }

    void reset(T &&value)
    {
        std::scoped_lock lock{mutex};
        dirty = true;
        this->value = value;
    }

    void reset(const T &value)
    {
        std::scoped_lock lock{mutex};
        dirty = true;
        this->value = value;
    }

    /**
     * get the value contained in the pollable, leaving the dirty flag intact.
     */
    T get() const
    {
        std::scoped_lock lock{mutex};
        return value;
    }

    /**
     * read the value from the pollable, clearing the dirty flag.
     */
    T read()
    {
        std::scoped_lock lock{mutex};
        dirty = false;
        return value;
    }

    void clean() {
        std::scoped_lock lock{mutex};
        dirty = false;
    }

    Pollable<T> &operator=(const T &t)
    {
        reset(t);
        return *this;
    }

    T operator*() const { return get(); }
    operator bool() const { return is_dirty(); }
};

// Deduction guide
template <typename T>
Pollable(T)->Pollable<T>;

#endif
