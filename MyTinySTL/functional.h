#ifndef MYTINYSTL_FUNCTIONAL_H_
#define MYTINYSTL_FUNCTIONAL_H_

// 包含mystl的函数对象与哈希函数

#include <cstddef>

namespace mystl {

// 定义一元函数的参数型别和返回值型别
template <typename Arg, typename Result>
struct UnargFunction {
  using argument_type = Arg;
  using result_type = Result;
};

// 定义二元函数的参数型别的返回值型别
template <typename Arg1, typename Arg2, typename Result>
struct BinaryFunction {
  using first_argument_type = Arg1;
  using second_argument_type = Arg2;
  using result_type = Result;
};

// 函数对象：加法
template <typename T>
struct Plus : public BinaryFunction<T, T, T> {
  T operator()(const T& x, const T& y) const { return x + y; }
};

// 函数对象：减法
template <typename T>
struct Minus : public BinaryFunction<T, T, T> {
  T operator()(const T& x, const T& y) const { return x - y; }
};

// 函数对象：乘法
template <typename T>
struct Multiplies : public BinaryFunction<T, T, T> {
  T operator()(const T& x, const T& y) const { return x * y; }
};

// 函数对象：除法
template <typename T>
struct Divides : public BinaryFunction<T, T, T> {
  T operator()(const T& x, const T& y) const { return x / y; }
};

// 函数对象：模取
template <typename T>
struct Modulus : public BinaryFunction<T, T, T> {
  T operator()(const T& x, const T& y) const { return x % y; }
};

// 函数对象：否定
template <typename T>
struct Negate : public UnargFunction<T, T> {
  T operator()(const T& x) const { return -x; }
};

// 加法的证同元素
template <typename T>
T identify_element(Plus<T> /*unused*/) {
  return T(0);
}

// 乘法的证同元素
template <typename T>
T identify_element(Multiplies<T> /*unused*/) {
  return T(1);
}

// 函数对象：等于
template <typename T>
struct EqualTo : public BinaryFunction<T, T, bool> {
  bool operator()(const T& x, const T& y) const { return x == y; }
};

// 函数对象：不等于
template <typename T>
struct NotEqualTo : public BinaryFunction<T, T, bool> {
  bool operator()(const T& x, const T& y) const { return x != y; }
};

// 函数对象：大于
template <typename T>
struct Greater : public BinaryFunction<T, T, bool> {
  bool operator()(const T& x, const T& y) const { return x > y; }
};

// 函数对象：小于
template <typename T>
struct Less : public BinaryFunction<T, T, bool> {
  bool operator()(const T& x, const T& y) const { return x < y; }
};

// 函数对象：大于等于
template <typename T>
struct GreaterEqual : public BinaryFunction<T, T, bool> {
  bool operator()(const T& x, const T& y) const { return x >= y; }
};

// 函数对象：小于等于
template <typename T>
struct LessEqual : public BinaryFunction<T, T, bool> {
  bool operator()(const T& x, const T& y) const { return x <= y; }
};

// 函数对象：逻辑与
template <typename T>
struct LogicalAnd : public BinaryFunction<T, T, bool> {
  bool operator()(const T& x, const T& y) const { return x && y; }
};

// 函数对象：逻辑或
template <typename T>
struct LogicalOr : public BinaryFunction<T, T, bool> {
  bool operator()(const T& x, const T& y) const { return x || y; }
};

// 函数对象：逻辑非
template <typename T>
struct LogicalNot : public UnargFunction<T, bool> {
  bool operator()(const T& x) const { return !x; }
};

// 证同函数：不会改变元素，返回本身
template <typename T>
struct Identity : public UnargFunction<T, bool> {
  const T& operator()(const T& x) const { return x; }
};

// 选择函数：接受一个pair，返回第一个元素
template <typename Pair>
struct SelectFirst : public UnargFunction<Pair, typename Pair::first_type> {
  const typename Pair::first_type& operator()(const Pair& x) const { return x.first; }
};

// 选择函数：接受一个pair，返回第二个元素
template <typename Pair>
struct SelectSecond : public UnargFunction<Pair, typename Pair::second_type> {
  const typename Pair::second_type& operator()(const Pair& x) const { return x.second; }
};

// 投射函数：返回第一参数
template <typename Arg1, typename Arg2>
struct ProjectFirst : public BinaryFunction<Arg1, Arg2, Arg1> {
  Arg1 operator()(const Arg1& x, const Arg2& /*unused*/) const { return x; }
};

// 投射函数：返回第二参数
template <typename Arg1, typename Arg2>
struct ProjectSecond : public BinaryFunction<Arg1, Arg2, Arg2> {
  Arg2 operator()(const Arg1& /*unused*/, const Arg2& y) const { return y; }
};

// 哈希函数对象

// 对于大部分类型，hash function什么都不做
template <typename Key>
struct Hash {};

// 针对指针的偏特化版本
template <typename T>
struct Hash<T*> {
  size_t operator()(T* p) const noexcept { return reinterpret_cast<size_t>(p); }
};

// 对于整型类型，只是返回原值
#define MYSTL_TRIVIAL_HASH_FCN(Type)                                                \
  template <>                                                                       \
  struct Hash<Type> {                                                               \
    size_t operator()(Type val) const noexcept { return static_cast<size_t>(val); } \
  };

MYSTL_TRIVIAL_HASH_FCN(bool)

MYSTL_TRIVIAL_HASH_FCN(char)

MYSTL_TRIVIAL_HASH_FCN(signed char)

MYSTL_TRIVIAL_HASH_FCN(unsigned char)

MYSTL_TRIVIAL_HASH_FCN(wchar_t)

MYSTL_TRIVIAL_HASH_FCN(char16_t)

MYSTL_TRIVIAL_HASH_FCN(char32_t)

MYSTL_TRIVIAL_HASH_FCN(short)

MYSTL_TRIVIAL_HASH_FCN(unsigned short)

MYSTL_TRIVIAL_HASH_FCN(int)

MYSTL_TRIVIAL_HASH_FCN(unsigned int)

MYSTL_TRIVIAL_HASH_FCN(long)

MYSTL_TRIVIAL_HASH_FCN(unsigned long)

MYSTL_TRIVIAL_HASH_FCN(long long)

MYSTL_TRIVIAL_HASH_FCN(unsigned long long)

#undef MYSTL_TRIVIAL_HASH_FCN

// 对于浮点数，逐位哈希
inline size_t bitwise_hash(const unsigned char* first, size_t count) {
#if (_MSC_VER && _WIN64) || ((__GNUC__ || __clang__) && __SIZEOF_POINTER__ == 8)
  const size_t fnv_offset = 14695981039346656037ull;
  const size_t fnv_prime = 1099511628211ull;
#else
  const size_t fnv_offset = 2166136261u;
  const size_t fnv_prime = 16777619u;
#endif
  size_t result = fnv_offset;
  for (size_t i = 0; i < count; ++i) {
    result ^= (size_t)first[i];
    result *= fnv_prime;
  }
  return result;
}

template <>
struct Hash<float> {
  size_t operator()(const float& val) {
    return val == 0.0f ? 0 : bitwise_hash((const unsigned char*)&val, sizeof(float));
  }
};

template <>
struct Hash<double> {
  size_t operator()(const double& val) {
    return val == 0.0f ? 0 : bitwise_hash((const unsigned char*)&val, sizeof(double));
  }
};

template <>
struct Hash<long double> {
  size_t operator()(const long double& val) {
    return val == 0.0f ? 0 : bitwise_hash((const unsigned char*)&val, sizeof(long double));
  }
};

}  // namespace mystl
#endif  // !MYTINYSTL_FUNCTIONAL_H_