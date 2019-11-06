#ifndef META_HPP
#define META_HPP

#include <type_traits>

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
