#ifndef MYTINYSTL_TYPE_TRAITS_H_
#define MYTINYSTL_TYPE_TRAITS_H_

// 用于提取类型信息
// 对应书3.4节

#include <type_traits>

namespace mystl {

template <typename T, T v>
struct MIntegralConstant {
  static constexpr T kValue = v;
};

template <bool b>
using m_bool_constant = MIntegralConstant<bool, b>;

using m_true_type = m_bool_constant<true>;
using m_false_type = m_bool_constant<false>;

// type traits

// is_pair

// forward declaration begin
template <typename T1, typename T2>
struct pair;
// forward declaration end

template <typename T>
struct IsPair : mystl::m_false_type {};

template <typename T1, class T2>
struct IsPair<mystl::pair<T1, T2>> : mystl::m_true_type {};

}  // namespace mystl

#endif  // !MYTINYSTL_TYPE_TRAITS_H_
