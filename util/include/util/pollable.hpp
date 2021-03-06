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
    // typename specifies that the expression does name a type, which cannot otherwise be inferred.
    typename std::conditional<atomic, std::mutex, std::nullptr_t>::type mutex;

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
