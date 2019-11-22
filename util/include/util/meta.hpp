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

#ifndef META_HPP
#define META_HPP

#include <type_traits>
#include <iterator>

namespace meta {

template <typename T, typename = void>
struct is_container : std::false_type {
};

template <typename T>
using element_type = decltype(std::begin(std::declval<T &>()));

template <typename T>
struct is_container<T, std::void_t<element_type<T>>> : std::true_type {
};
template <typename T>
constexpr auto is_container_v = is_container<T>::value;

template <typename T, typename = void>
struct is_string : std::false_type {
};

template <typename T>
struct is_string<T, std::enable_if_t<is_container<T>::value>>
    : std::is_same<element_type<T>, char> {
};

// incomplete type to allow error printing
template <typename Err>
struct unsupported;

} // namespace meta

#endif
