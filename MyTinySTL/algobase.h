#ifndef MYTINYSTL_ALGOBASE_H_
#define MYTINYSTL_ALGOBASE_H_

// 包含基本算法

#include <cstring>

#include "iterator.h"
#include "util.h"

namespace mystl {

#ifdef max
#pragma message("#undefing marco max")
#undef max
#endif  // max

#ifdef min
#pragma message("#undefing marco min")
#undef min
#endif  // min

// max
// 取二者中的较大值，语义相等时保证返回第一个参数
template <typename T>
const T& max(const T& lhs, const T& rhs) {
  return lhs < rhs ? rhs : lhs;
}

// 重载版本使用函数对象comp代替比较操作
template <typename T, typename Compare>
const T& max(const T& lhs, const T& rhs, Compare comp) {
  return comp(lhs, rhs) ? rhs : lhs;
}

// min
// 取二者中的较小值，语义相等时保证返回第一个参数
template <typename T>
const T& min(const T& lhs, const T& rhs) {
  return rhs < lhs ? rhs : lhs;
}

// 重载版本使用函数对象comp代替比较操作
template <typename T, typename Compare>
const T& min(const T& lhs, const T& rhs, Compare comp) {
  return comp(rhs, lhs) ? rhs : lhs;
}

// iter_swap
// 将两个迭代器所指对象对调
template <typename FIter1, typename FIter2>
void iter_swap(FIter1 lhs, FIter2 rhs) {
  mystl::swap(*lhs, *rhs);
}

// copy
// 把[first, last)区间内的元素拷贝到[result, result + (last - first))内

// InputIteratorTag版本
template <typename InputIter, typename OutputIter>
OutputIter unchecked_copy_cat(
    InputIter first, InputIter last, OutputIter result, mystl::InputIteratorTag /*unused*/) {
  for (; first != last; ++first, ++result) {
    *result = *first;
  }
  return result;
}

// RandomAccessIteratorTag版本
template <typename RandomIter, typename OutputIter>
OutputIter unchecked_copy_cat(
    RandomIter first,
    RandomIter last,
    OutputIter result,
    mystl::RandomAccessIteratorTag /*unused*/) {
  for (auto n = last - first; n > 0; --n, ++first, ++result) {
    *result = *first;
  }
  return result;
}

template <typename InputIter, typename OutputIter>
OutputIter unchecked_copy(InputIter first, InputIter last, OutputIter result) {
  return unchecked_copy_cat(first, last, result, iterator_category(first));
}

// 为trivially_copy_assignable类型提供特化版本
template <typename Tp, typename Up>
typename std::enable_if<
    std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
        std::is_trivially_copy_assignable<Up>::value,
    Up*>::type
unchecked_copy(Tp* first, Tp* last, Up* result) {
  const auto n = static_cast<size_t>(last - first);
  if (n != 0) {
    std::memmove(result, first, n * sizeof(Up));
  }
  return result + n;
}

template <typename InputIter, typename OutputIter>
OutputIter copy(InputIter first, InputIter last, OutputIter result) {
  return unchecked_copy(first, last, result);
}

// copy_backward
// 将[first, last)区间内的元素拷贝到[result - (last - first), result)内

// unchecked_copy_backward_cat的BidirectionalIteratorTag版本
template <typename BidirectionalIter1, typename BidirectionalIter2>
BidirectionalIter2 unchecked_copy_backward_cat(
    BidirectionalIter1 first,
    BidirectionalIter1 last,
    BidirectionalIter2 result,
    mystl::BidirectionalIteratorTag /*unused*/) {
  while (first != last) {
    *--result = *--last;
  }
  return result;
}

// unchecked_copy_backward_cat的RandomAccessIteratorTag版本
template <typename RandomIter1, typename BidirectionalIter2>
BidirectionalIter2 unchecked_copy_backward_cat(
    RandomIter1 first,
    RandomIter1 last,
    BidirectionalIter2 result,
    mystl::RandomAccessIteratorTag /*unused*/) {
  for (auto n = last - first; n > 0; --n) {
    *--result = *--last;
  }
  return result;
}

template <typename BidirectionalIter1, typename BidirectionalIter2>
BidirectionalIter2 unchecked_copy_backward(
    BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result) {
  return unchecked_copy_backward_cat(first, last, result, iterator_category(first));
}

// 为trivially_copy_assignable类型提供特化版本
template <typename Tp, typename Up>
typename std::enable_if<
    std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
        std::is_trivially_copy_assignable<Up>::value,
    Up*>::type
unchecked_copy_backward(Tp* first, Tp* last, Up* result) {
  const auto n = static_cast<size_t>(last - first);
  if (n != 0) {
    result -= n;
    std::memmove(result, first, n * sizeof(Up));
  }
  return result;
}

template <typename BidirectionalIter1, typename BidirectionalIter2>
BidirectionalIter2 copy_backward(
    BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result) {
  return unchecked_copy_backward(first, last, result);
}

// copy_n
// 把[first, first + n)区间上的元素拷贝到[result, result + n)上
// 返回一个pair分别指向拷贝结束的尾部
template <typename InputIter, typename Size, typename OutputIter>
mystl::pair<InputIter, OutputIter> unchecked_copy_n(
    InputIter first, Size n, OutputIter result, mystl::InputIteratorTag /*unused*/) {
  for (; n > 0; --n, ++first, ++result) {
    *result = *first;
  }
  return mystl::pair<InputIter, OutputIter>(first, result);
}

template <typename RandomIter, typename Size, typename OutputIter>
mystl::pair<RandomIter, OutputIter> unchecked_copy_n(
    RandomIter first, Size n, OutputIter result, mystl::RandomAccessIteratorTag /*unused*/) {
  auto last = first + n;
  return mystl::pair<RandomIter, OutputIter>(last, mystl::copy(first, last, result));
}

template <typename InputIter, typename Size, typename OutputIter>
mystl::pair<InputIter, OutputIter> copy_n(InputIter first, Size n, OutputIter result) {
  return unchecked_copy_n(first, n, result, iterator_category(first));
}

// fill_n
// 从first位置开始填充n个值
template <typename OutputIter, typename Size, typename T>
OutputIter unchecked_fill_n(OutputIter first, Size n, const T& value) {
  for (; n > 0; --n, ++first) {
    *first = value;
  }
  return first;
}

// 为one-byte类型提供特化版本
template <typename Tp, typename Size, typename Up>
typename std::enable_if<
    std::is_integral<Tp>::value && sizeof(Tp) == 1 && !std::is_same<Tp, bool>::value &&
        std::is_integral<Up>::value && sizeof(Up) == 1,
    Tp*>::type
unchecked_fill_n(Tp* first, Size n, Up value) {
  if (n > 0) {
    std::memset(first, (unsigned char)value, (size_t)(n));
  }
  return first + n;
}

template <typename OutputIter, typename Size, typename T>
OutputIter fill_n(OutputIter first, Size n, const T& value) {
  return unchecked_fill_n(first, n, value);
}

// fill
// 为[first, last)区间内的所有元素填充新值
template <typename ForwardIter, typename T>
void fill_cat(
    ForwardIter first, ForwardIter last, const T& value, mystl::ForwardIteratorTag /*unused*/) {
  for (; first != last; ++first) {
    *first = value;
  }
}

template <typename RandomIter, typename T>
void fill_cat(
    RandomIter first, RandomIter last, const T& value, mystl::RandomAccessIteratorTag /*unused*/) {
  fill_n(first, last - first, value);
}

template <typename ForwardIter, typename T>
void fill(ForwardIter first, ForwardIter last, const T& value) {
  fill_cat(first, last, value, iterator_category(first));
}

// move
// 把[first, last)区间内的元素移动到[result, result + (last - first))内
template <typename InputIter, typename OutputIter>
OutputIter unchecked_move_cat(
    InputIter first, InputIter last, OutputIter result, mystl::InputIteratorTag /*unused*/) {
  for (; first != last; ++first, ++result) {
    *result = mystl::move(*first);
  }
  return result;
}

template <typename RandomIter, typename OutputIter>
OutputIter unchecked_move_cat(
    RandomIter first,
    RandomIter last,
    OutputIter result,
    mystl::RandomAccessIteratorTag /*unused*/) {
  for (auto n = last - first; n >= 0; --n, ++first, ++result) {
    *result = mystl::move(*first);
  }
  return result;
}

template <typename InputIter, typename OutputIter>
OutputIter unchecked_move(InputIter first, InputIter last, OutputIter result) {
  return unchecked_move_cat(first, last, result, iterator_category(first));
}

// 为trivially_move_assignable类型提供特化版本
template <typename Tp, typename Up>
typename std::enable_if<
    std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
        std::is_trivially_move_assignable<Up>::value,
    Up*>::type
unchecked_move(Tp* first, Tp* last, Up* result) {
  const auto n = static_cast<size_t>(last - first);
  if (n != 0) {
    std::memmove(result, first, n * sizeof(Up));
  }
  return result + n;
}

template <typename InputIter, typename OutputIter>
OutputIter move(InputIter first, InputIter last, OutputIter result) {
  return unchecked_move(first, last, result);
}

// move_backward
// 将[first, last)区间内的元素移动到[result - (last - first), result)内

// BidirectionalIteratorTag版本
template <typename BidirectionalIter1, typename BidirectionalIter2>
BidirectionalIter2 unchecked_move_backward_cat(
    BidirectionalIter1 first,
    BidirectionalIter1 last,
    BidirectionalIter2 result,
    mystl::BidirectionalIteratorTag /*unused*/) {
  while (first != last) {
    *--result = mystl::move(*--last);
  }
  return result;
}

// RandomAccessIteratorTag版本
template <typename RandomIter1, typename RandomIter2>
RandomIter2 unchecked_move_backward_cat(
    RandomIter1 first,
    RandomIter1 last,
    RandomIter2 result,
    mystl::RandomAccessIteratorTag /*unused*/) {
  for (auto n = last - first; n > 0; --n) {
    *--result = mystl::move(*--last);
  }
  return result;
}

template <typename BidirectionalIter1, typename BidirectionalIter2>
BidirectionalIter2 unchecked_move_backward(
    BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result) {
  return unchecked_copy_backward_cat(first, last, result, iterator_category(first));
}

// 为trivially_copy_assignable类型提供特化版本
template <typename Tp, typename Up>
typename std::enable_if<
    std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
        std::is_trivially_move_assignable<Up>::value,
    Up*>::type
unchecked_move_backward(Tp* first, Tp* last, Up* result) {
  const auto n = static_cast<size_t>(last - first);
  if (n != 0) {
    result -= n;
    std::memmove(result, first, n * sizeof(Up));
  }
  return result;
}

template <typename BidirectionalIter1, typename BidirectionalIter2>
BidirectionalIter2 move_backward(
    BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result) {
  return unchecked_copy_backward(first, last, result);
}

// equal
// 比较第一序列在[first, last)区间上的元素值是否和第二个序列相等
template <typename InputIter1, typename InputIter2>
bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2) {
  for (; first1 != last1; ++first1, ++first2) {
    if (*first1 != *first2) {
      return false;
    }
  }
  return true;
}

// 重载版本
template <typename InputIter1, typename InputIter2, typename Compared>
bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2, Compared comp) {
  for (; first1 != last1; ++first1, ++first2) {
    if (!comp(*first1, *first2)) {
      return false;
    }
  }
  return true;
}

// lexicographical_compare
// 字典序比较
template <typename InputIter1, typename InputIter2>
bool lexicographical_compare(
    InputIter1 first1, InputIter1 last1, InputIter2 first2, InputIter2 last2) {
  for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
    if (*first1 < *first2) {
      return true;
    }
    if (*first2 < *first1) {
      return false;
    }
  }
  return first1 == last1 && first2 != last2;
}

// 重载版本
template <typename InputIter1, typename InputIter2, typename Compared>
bool lexicographical_compare(
    InputIter1 first1, InputIter1 last1, InputIter2 first2, InputIter2 last2, Compared comp) {
  for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
    if (comp(*first1, *first2)) {
      return true;
    }
    if (comp(*first2, *first1)) {
      return false;
    }
  }
  return first1 == last1 && first2 != last2;
}

// 针对const unsigned char*的特化版本
bool lexicographical_compare(
    const unsigned char* first1,
    const unsigned char* last1,
    const unsigned char* first2,
    const unsigned char* last2) {
  const auto len1 = last1 - first1;
  const auto len2 = last2 - first2;
  // 先比较相同长度的部分
  const auto result = std::memcmp(first1, first2, mystl::min(len1, len2));
  // 若相等，长度较大的比较大
  return result != 0 ? result < 0 : len1 < len2;
}

}  // namespace mystl

#endif  // !MYTINYSTL_ALGOBASE_H_