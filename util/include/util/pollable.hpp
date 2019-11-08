#ifndef UTIL_POLLABLE_HPP
#define UTIL_POLLABLE_HPP

/**
 * Wrapper class around a value that can be updated from elsewhere and be polled for
 * whether such updates have happened.
 */
template <typename T>
class Pollable {
    T value;
    bool dirty = false;

public:
    Pollable() = default;
    Pollable(T &&value) : value(value), dirty(false) {}

    Pollable(const Pollable<T> &) = default;
    Pollable(Pollable<T> &&) = default;
    Pollable<T> &operator=(Pollable<T> &&) = default;
    Pollable<T> &operator=(const Pollable<T> &) = default;

    bool is_dirty() const { return dirty; }

    void reset(T &&value)
    {
        dirty = true;
        this->value = value;
    }

    void reset(const T &value)
    {
        dirty = true;
        this->value = value;
    }

    T get()
    {
        dirty = false;
        return value;
    }

    Pollable<T> &operator=(const T &t)
    {
        reset(t);
        return *this;
    }

    T operator*() { return get(); }
    operator bool() const { return dirty; }
};

// Deduction guide
template <typename T>
Pollable(T) -> Pollable<T>;

#endif
