#ifndef MYTINYSTL_UTIL_H_
#define MYTINYSTL_UTIL_H_

// 包含一些通用工具

#include <cstddef>

#include "type_traits.h"

namespace mystl {

// move

template <typename T>
typename std::remove_reference<T>::type&& move(T&& arg) noexcept {
  return static_cast<typename std::remove_reference<T>::type&&>(arg);
}

// forward

template <typename T>
T&& forward(typename std::remove_reference<T>::type& arg) noexcept {
  return static_cast<T&&>(arg);
}

template <typename T>
T&& forward(typename std::remove_reference<T>::type&& arg) noexcept {
  static_assert(!std::is_lvalue_reference<T>::value, "bad forward");
  return static_cast<T&&>(arg);
}

// swap

template <typename Tp>
void swap(Tp& lhs, Tp& rhs) {
  auto tmp(mystl::move(lhs));
  lhs = mystl::move(rhs);
  rhs = mystl::move(tmp);
}

template <typename ForwardIter1, typename ForwardIter2>
ForwardIter2 swap_range(ForwardIter1 first1, ForwardIter1 last1, ForwardIter2 first2) {
  for (; first1 != last1; ++first2, (void)++first2) {
    mystl::swap(*first1, *first2);
  }
  return first2;
}

template <typename Tp, size_t N>
void swap(Tp (&a)[N], Tp (&b)[N]) {
  mystl::swap_range(a, a + N, b);
}

// pair

// 结构体模板
// 两个模板参数分别表示两个数据的类型
// 用first和second来分别去除第一个和第二个数据
template <typename Ty1, typename Ty2>
struct pair {
  using first_type = Ty1;
  using second_type = Ty2;

  first_type first;
  second_type second;

  template <
      typename Other1 = Ty1,
      typename Other2 = Ty2,
      typename = typename std::enable_if<
          std::is_default_constructible<Other1>::value &&
              std::is_default_constructible<Other2>::value,
          void>::type>
  constexpr pair() : first(), second() {}

  template <
      typename U1 = Ty1,
      typename U2 = Ty2,
      typename std::enable_if<
          std::is_copy_constructible<U1>::value && std::is_copy_constructible<U2>::value &&
              std::is_convertible<const U1&, Ty1>::value &&
              std::is_convertible<const U2&, Ty2>::value,
          int>::type = 0>
  constexpr pair(const Ty1& a, const Ty2& b) : first(a), second(b) {}

  template <
      typename U1 = Ty1,
      typename U2 = Ty2,
      typename std::enable_if<
          std::is_copy_constructible<U1>::value && std::is_copy_constructible<U2>::value &&
              (!std::is_convertible<const U1&, Ty1>::value ||
               !std::is_convertible<const U2&, Ty2>::value),
          int>::type = 0>
  explicit constexpr pair(const Ty1& a, const Ty2& b) : first(a), second(b) {}

  pair(const pair& rhs) = default;
  pair(pair&& rhs) noexcept = default;

  template <
      typename Other1,
      typename Other2,
      typename std::enable_if<
          std::is_constructible<Ty1, Other1>::value && std::is_constructible<Ty2, Other2>::value &&
              std::is_convertible<Other1&&, Ty1>::value &&
              std::is_convertible<Other2&&, Ty2>::value,
          int>::type = 0>
  constexpr pair(Other1&& a, Other2&& b)
      : first(mystl::forward<Other1>(a)), second(mystl::forward<Other2>(b)) {}

  template <
      typename Other1,
      typename Other2,
      typename std::enable_if<
          std::is_constructible<Ty1, Other1>::value && std::is_constructible<Ty2, Other2>::value &&
              (!std::is_convertible<Other1, Ty1>::value ||
               !std::is_convertible<Other2, Ty2>::value),
          int>::type = 0>
  explicit constexpr pair(Other1&& a, Other2&& b)
      : first(mystl::forward<Other1>(a)), second(mystl::forward<Other2>(b)) {}

  template <
      typename Other1,
      typename Other2,
      typename std::enable_if<
          std::is_constructible<Ty1, const Other1&>::value &&
              std::is_constructible<Ty2, const Other2&>::value &&
              std::is_convertible<const Other1&, Ty1>::value &&
              std::is_convertible<const Other2&, Ty2>::value,
          int>::type = 0>
  constexpr pair(const pair<Other1, Other2>& other) : first(other.first), second(other.second) {}

  template <
      typename Other1,
      typename Other2,
      typename std::enable_if<
          std::is_constructible<Ty1, const Other1&>::value &&
              std::is_constructible<Ty2, const Other2&>::value &&
              (!std::is_convertible<const Other1&, Ty1>::value ||
               !std::is_convertible<const Other2&, Ty2>::value),
          int>::type = 0>
  explicit constexpr pair(const pair<Other1, Other2>& other)
      : first(other.first), second(other.second) {}

  template <
      typename Other1,
      typename Other2,
      typename std::enable_if<
          std::is_constructible<Ty1, Other1>::value && std::is_constructible<Ty2, Other2>::value &&
              std::is_convertible<Other1, Ty1>::value && std::is_convertible<Other2, Ty2>::value,
          int>::type = 0>
  constexpr pair(pair<Other1, Other2>&& other)
      : first(mystl::forward<Other1>(other.first)), second(mystl::forward<Other2>(other.second)) {}

  template <
      typename Other1,
      typename Other2,
      typename std::enable_if<
          std::is_constructible<Ty1, Other1>::value && std::is_constructible<Ty2, Other2>::value &&
              (!std::is_convertible<Other1, Ty1>::value ||
               !std::is_convertible<Other2, Ty2>::value),
          int>::type = 0>
  explicit constexpr pair(pair<Other1, Other2>&& other)
      : first(mystl::forward<Other1>(other.first)), second(mystl::forward<Other2>(other.second)) {}

  pair& operator=(const pair& rhs) {
    if (this != &rhs) {
      first = rhs.first;
      second = rhs.second;
    }
    return *this;
  }

  pair& operator=(pair&& rhs) noexcept {
    if (this != &rhs) {
      first = mystl::move(rhs.first);
      second = mystl::move(rhs.second);
    }
    return *this;
  }

  template <typename Other1, typename Other2>
  pair& operator=(const pair<Other1, Other2>& other) {
    first = other.first;
    second = other.second;
    return *this;
  }

  template <typename Other1, typename Other2>
  pair& operator=(pair<Other1, Other2>&& other) {
    first = mystl::forward<Other1>(other.first);
    second = mystl::forward<Other2>(other.second);
    return *this;
  }

  ~pair() = default;

  void swap(pair& other) {
    if (this != &other) {
      mystl::swap(first, other.first);
      mystl::swap(second, other.second);
    }
  }
};

// 重载比较操作符
template <typename Ty1, typename Ty2>
bool operator==(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs) {
  return lhs.first == rhs.first && lhs.second == rhs.second;
}

template <typename Ty1, typename Ty2>
bool operator<(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs) {
  return lhs.first < rhs.first || (lhs.first == rhs.first && lhs.second < rhs.second);
}

template <typename Ty1, typename Ty2>
bool operator!=(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs) {
  return !(lhs == rhs);
}

template <typename Ty1, typename Ty2>
bool operator>(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs) {
  return rhs < lhs;
}

template <typename Ty1, typename Ty2>
bool operator<=(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs) {
  return !(rhs < lhs);
}

template <typename Ty1, typename Ty2>
bool operator>=(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs) {
  return !(lhs < rhs);
}

// 重载mystl的swap
template <typename Ty1, typename Ty2>
void swap(pair<Ty1, Ty2>& lhs, pair<Ty1, Ty2>& rhs) {
  lhs.swap(rhs);
}

// 全局函数，让两个数据成为一个pair
template <typename Ty1, typename Ty2>
pair<Ty1, Ty2> make_pair(Ty1&& first, Ty2&& second) {
  return pair<Ty1, Ty2>(mystl::forward<Ty1>(first), mystl::forward<Ty2>(second));
}

}  // namespace mystl

#endif  // !MYTINYSTL_UTIL_H_