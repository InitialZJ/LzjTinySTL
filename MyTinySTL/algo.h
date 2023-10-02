#ifndef MYTINYSTL_ALGO_H_
#define MYTINYSTL_ALGO_H_

#include "iterator.h"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)
#endif

// 包含一系列算法
// 对应书6.7节

#include <cstddef>
#include <ctime>

#include "algobase.h"
#include "functional.h"
#include "heap_algo.h"
#include "memory.h"

namespace mystl {

// all_of
// 检查[first, last)内是否全部元素都满足一元操作unary_pred为true的情况，满足则返回true
template <typename InputIter, typename UnaryPredicate>
bool all_of(InputIter first, InputIter last, UnaryPredicate unary_pred) {
  for (; first != last; ++first) {
    if (!unary_pred(*first)) {
      return false;
    }
  }
  return true;
}

// any_of
// 检查[first, last)内是否存在某个元素都满足一元操作unary_pred为true的情况，满足则返回true
template <typename InputIter, typename UnaryPredicate>
bool any_of(InputIter first, InputIter last, UnaryPredicate unary_pred) {
  for (; first != last; ++first) {
    if (unary_pred(*first)) {
      return true;
    }
  }
  return false;
}

// none_of
// 检查[first, last)内是否全部元素都不满足一元操作unary_pred为true的情况，满足则返回true
template <typename InputIter, typename UnaryPredicate>
bool none_of(InputIter first, InputIter last, UnaryPredicate unary_pred) {
  for (; first != last; ++first) {
    if (unary_pred(*first)) {
      return false;
    }
  }
  return true;
}

// count
// 对[first, last)区间内的元素与给定值进行比较，缺省使用operator==，返回元素相等的个数
template <typename InputIter, typename T>
size_t count(InputIter first, InputIter last, const T& value) {
  size_t n = 0;
  for (; first != last; ++first) {
    if (*first == value) {
      ++n;
    }
  }
  return n;
}

// count_if
// 对[first, last)区间内的每个元素都进行一元unary_pred操作，返回结果为true的个数
template <typename InputIter, typename UnaryPredicate>
size_t count_if(InputIter first, InputIter last, UnaryPredicate unary_pred) {
  size_t n = 0;
  for (; first != last; ++first) {
    if (unary_pred(*first)) {
      ++n;
    }
  }
  return n;
}

// find
// 在[first, last)区间内找到等于value的元素，返回指向该元素的迭代器
template <typename InputIter, typename T>
InputIter find(InputIter first, InputIter last, const T& value) {
  while (first != last && *first != value) {
    ++first;
  }
  return first;
}

// find_if
// 在[first, last)区间内找到第一个另一元操作unary_pred为true的元素，返回指向该元素的迭代器
template <typename InputIter, typename UnaryPredicate>
InputIter find_if(InputIter first, InputIter last, UnaryPredicate unary_pred) {
  while (first != last && !unary_pred(*first)) {
    ++first;
  }
  return first;
}

// find_if_not
// 在[first, last)区间内找到第一个另一元操作unary_pred为false的元素，返回指向该元素的迭代器
template <typename InputIter, typename UnaryPredicate>
InputIter find_if_not(InputIter first, InputIter last, UnaryPredicate unary_pred) {
  while (first != last && unary_pred(*first)) {
    ++first;
  }
  return first;
}

// search
// 在[first1, last1)中查找[first2, last2)的首次出现点
template <typename ForwardIter1, typename ForwardIter2>
ForwardIter1 search(
    ForwardIter1 first1, ForwardIter1 last1, ForwardIter2 first2, ForwardIter2 last2) {
  auto d1 = mystl::distance(first1, last1);
  auto d2 = mystl::distance(first2, last2);
  if (d1 < d2) {
    return last1;
  }
  auto current1 = first1;
  auto current2 = first2;
  while (current2 != last2) {
    if (*current1 == *current2) {
      ++current1;
      ++current2;
    } else {
      if (d1 == d2) {
        return last1;
      } else {
        current1 = ++first1;
        current2 = first2;
        --d1;
      }
    }
  }
  return first1;
}

// 重载版本使用函数对象comp代替比较操作
template <typename ForwardIter1, typename ForwardIter2, typename Compared>
ForwardIter1 search(
    ForwardIter1 first1,
    ForwardIter1 last1,
    ForwardIter2 first2,
    ForwardIter2 last2,
    Compared comp) {
  auto d1 = mystl::distance(first1, last1);
  auto d2 = mystl::distance(first2, last2);
  if (d1 < d2) {
    return last1;
  }
  auto current1 = first1;
  auto current2 = first2;
  while (current2 != last2) {
    if (comp(*current1, *current2)) {
      ++current1;
      ++current2;
    } else {
      if (d1 == d2) {
        return last1;
      } else {
        current1 = ++first1;
        current2 = first2;
        --d1;
      }
    }
  }
  return first1;
}

// search_n
// 在[first, last)中查找连续n个value所形成的子序列，返回一个迭代器指向该子序列的起始处
template <typename ForwardIter, typename Size, typename T>
ForwardIter search_n(ForwardIter first, ForwardIter last, Size n, const T& value) {
  if (n <= 0) {
    return first;
  } else {
    first = mystl::find(first, last, value);
    while (first != last) {
      auto m = n - 1;
      auto i = first;
      ++i;
      while (i != last && m != 0 && *i == value) {
        ++i;
        --m;
      }
      if (m == 0) {
        return first;
      } else {
        first = mystl::find(i, last, value);
      }
    }
    return last;
  }
}

// 重载版本使用函数对象comp代替比较操作
template <typename ForwardIter, typename Size, typename T, typename Compared>
ForwardIter search_n(ForwardIter first, ForwardIter last, Size n, const T& value, Compared comp) {
  if (n <= 0) {
    return first;
  } else {
    while (first != last) {
      if (comp(*first, value)) {
        break;
      }
      ++first;
    }
    while (first != last) {
      auto m = n - 1;
      auto i = first;
      ++i;
      while (i != last && m != 0 && comp(*i, value)) {
        ++i;
        --m;
      }
      if (m == 0) {
        return first;
      } else {
        while (i != last) {
          if (comp(*i, value)) {
            break;
          }
          ++i;
        }
        first = i;
      }
    }
    return last;
  }
}

// find_end
// 在[first1, last1)区间中查找[first2, last2)最后一次出现的地方，若不存在返回last1

// find_end_dispatch的forward_iterator_tag版本
template <typename ForwardIter1, typename ForwardIter2>
ForwardIter1 find_end_dispatch(
    ForwardIter1 first1,
    ForwardIter1 last1,
    ForwardIter2 first2,
    ForwardIter2 last2,
    ForwardIteratorTag /*tag*/,
    ForwardIteratorTag /*tag*/) {
  if (first2 == last2) {
    return last1;
  } else {
    auto result = last1;
    while (true) {
      // 利用search查找某个子序列的首次出现点，找不到则返回last1
      auto new_result = mystl::search(first1, last1, first2, last2);
      if (new_result == last1) {
        return result;
      } else {
        result = new_result;
        first1 = new_result;
        ++first1;
      }
    }
  }
}

// find_end_dispatch的bidirectional_iterator_tag版本
template <typename BidirectionalIter1, typename BidirectionalIter2>
BidirectionalIter1 find_end_dispatch(
    BidirectionalIter1 first1,
    BidirectionalIter1 last1,
    BidirectionalIter2 first2,
    BidirectionalIter2 last2,
    BidirectionalIteratorTag /*tag*/,
    BidirectionalIteratorTag /*tag*/) {
  using reviter1 = ReverseIterator<BidirectionalIter1>;
  using reviter2 = ReverseIterator<BidirectionalIter2>;
  reviter1 rlast1(first1);
  reviter2 rlast2(first2);
  reviter1 rresult = mystl::search(reviter1(last1), rlast1, reviter2(last2), rlast2);
  if (rresult == rlast1) {
    return last1;
  } else {
    auto result = rresult.base();
    mystl::advance(result, -mystl::distance(first2, last2));
    return result;
  }
}

template <typename ForwardIter1, typename ForwardIter2>
ForwardIter1 find_end(
    ForwardIter1 first1, ForwardIter1 last1, ForwardIter2 first2, ForwardIter2 last2) {
  using Category1 = typename IteratorTraits<ForwardIter1>::iterator_category;
  using Category2 = typename IteratorTraits<ForwardIter2>::iterator_category;
  return mystl::find_end_dispatch(first1, last1, first2, last2, Category1(), Category2());
}

// find_end重载comp版本

// find_end_dispatch的forward_iterator_tag版本
template <typename ForwardIter1, typename ForwardIter2, typename Compared>
ForwardIter1 find_end_dispatch(
    ForwardIter1 first1,
    ForwardIter1 last1,
    ForwardIter2 first2,
    ForwardIter2 last2,
    ForwardIteratorTag /*tag*/,
    ForwardIteratorTag /*tag*/,
    Compared comp) {
  if (first2 == last2) {
    return last1;
  } else {
    auto result = last1;
    while (true) {
      // 利用search查找某个子序列的首次出现点，找不到则返回last1
      auto new_result = mystl::search(first1, last1, first2, last2, comp);
      if (new_result == last1) {
        return result;
      } else {
        result = new_result;
        first1 = new_result;
        ++first1;
      }
    }
  }
}

// find_end_dispatch的bidirectional_iterator_tag版本
template <typename BidirectionalIter1, typename BidirectionalIter2, typename Compared>
BidirectionalIter1 find_end_dispatch(
    BidirectionalIter1 first1,
    BidirectionalIter1 last1,
    BidirectionalIter2 first2,
    BidirectionalIter2 last2,
    BidirectionalIteratorTag /*tag*/,
    BidirectionalIteratorTag /*tag*/,
    Compared comp) {
  using reviter1 = ReverseIterator<BidirectionalIter1>;
  using reviter2 = ReverseIterator<BidirectionalIter2>;
  reviter1 rlast1(first1);
  reviter2 rlast2(first2);
  reviter1 rresult = mystl::search(reviter1(last1), rlast1, reviter2(last2), rlast2, comp);
  if (rresult == rlast1) {
    return last1;
  } else {
    auto result = rresult.base();
    mystl::advance(result, -mystl::distance(first2, last2));
    return result;
  }
}

template <typename ForwardIter1, typename ForwardIter2, typename Compared>
ForwardIter1 find_end(
    ForwardIter1 first1,
    ForwardIter1 last1,
    ForwardIter2 first2,
    ForwardIter2 last2,
    Compared comp) {
  using Category1 = typename IteratorTraits<ForwardIter1>::iterator_category;
  using Category2 = typename IteratorTraits<ForwardIter2>::iterator_category;
  return mystl::find_end_dispatch(first1, last1, first2, last2, Category1(), Category2(), comp);
}

// find_first_of
// 在[first1, last1)中查找[first2, last2)中的某些元素，返回第一次出现的元素的迭代器
template <typename InputIter, typename ForwardIter>
InputIter find_first_of(InputIter first1, InputIter last1, ForwardIter first2, ForwardIter last2) {
  for (; first1 != last1; ++first1) {
    for (auto iter = first2; iter != last2; ++iter) {
      if (*first1 == *iter) {
        return first1;
      }
    }
  }
  return last1;
}

// 重载comp
template <typename InputIter, typename ForwardIter, typename Compared>
InputIter find_first_of(
    InputIter first1, InputIter last1, ForwardIter first2, ForwardIter last2, Compared comp) {
  for (; first1 != last1; ++first1) {
    for (auto iter = first2; iter != last2; ++iter) {
      if (comp(*first1, *iter)) {
        return first1;
      }
    }
  }
  return last1;
}

// for_each
// 使用一个函数对象f对[first, last)区间内的每个元素执行一个operator()操作，但不能改变元素内容
// f()可返回一个值，但该值会被忽略
template <typename InputIter, typename Function>
Function for_each(InputIter first, InputIter last, Function f) {
  for (; first != last; ++first) {
    f(*first);
  }
  return f;
}

// adjacent_find
// 找出第一对匹配的相邻元素，缺省使用operator==比较，如果找到返回一个迭代器，指向这对元素的第一个元素
template <typename ForwardIter>
ForwardIter adjacent_find(ForwardIter first, ForwardIter last) {
  if (first == last) {
    return last;
  }
  auto next = first;
  while (++next != last) {
    if (*first == *last) {
      return first;
    }
    first = next;
  }
  return last;
}

// 重载comp
template <typename ForwardIter, typename Compared>
ForwardIter adjacent_find(ForwardIter first, ForwardIter last, Compared comp) {
  if (first == last) {
    return last;
  }
  auto next = first;
  while (++next != last) {
    if (comp(*first, *last)) {
      return first;
    }
    first = next;
  }
  return last;
}

// lower_bound
// 在[first, last)中查找第一个不小于value的元素，并返回指向它的迭代器，若没有则返回last
// lbound_dispatch的ForwardIteratorTag版本
template <typename ForwardIter, typename T>
ForwardIter lbound_dispatch(
    ForwardIter first, ForwardIter last, const T& value, ForwardIteratorTag /*unused*/) {
  auto len = mystl::distance(first, last);
  auto half = len;
  ForwardIter middle;
  while (len > 0) {
    half = len >> 1;
    middle = first;
    mystl::advance(first, middle);
    if (*middle < value) {
      first = middle;
      ++first;
      len = len - half - 1;
    } else {
      len = half;
    }
  }
  return first;
}

// lbound_dispatch的RandomAccessIteratorTag版本
template <typename RandomIter, typename T>
RandomIter lbound_dispatch(
    RandomIter first, RandomIter last, const T& value, RandomAccessIteratorTag /*unused*/) {
  auto len = mystl::distance(first, last);
  auto half = len;
  RandomIter middle;
  while (len > 0) {
    half = len >> 1;
    middle = first + half;
    if (*middle < value) {
      first = middle + 1;
      len = len - half - 1;
    } else {
      len = half;
    }
  }
  return first;
}

template <typename ForwardIter, typename T>
ForwardIter lower_bound(ForwardIter first, ForwardIter last, const T& value) {
  return mystl::lbound_dispatch(first, last, value, iterator_category(first));
}

// lower_bound重载comp
// lbound_dispatch的ForwardIteratorTag版本
template <typename ForwardIter, typename T, typename Compared>
ForwardIter lbound_dispatch(
    ForwardIter first,
    ForwardIter last,
    const T& value,
    ForwardIteratorTag /*unused*/,
    Compared comp) {
  auto len = mystl::distance(first, last);
  auto half = len;
  ForwardIter middle;
  while (len > 0) {
    half = len >> 1;
    middle = first;
    mystl::advance(first, middle);
    if (comp(*middle, value)) {
      first = middle;
      ++first;
      len = len - half - 1;
    } else {
      len = half;
    }
  }
  return first;
}

// lbound_dispatch的RandomAccessIteratorTag版本
template <typename RandomIter, typename T, typename Compared>
RandomIter lbound_dispatch(
    RandomIter first,
    RandomIter last,
    const T& value,
    RandomAccessIteratorTag /*unused*/,
    Compared comp) {
  auto len = mystl::distance(first, last);
  auto half = len;
  RandomIter middle;
  while (len > 0) {
    half = len >> 1;
    middle = first + half;
    if (comp(*middle, value)) {
      first = middle + 1;
      len = len - half - 1;
    } else {
      len = half;
    }
  }
  return first;
}

template <typename ForwardIter, typename T, typename Compared>
ForwardIter lower_bound(ForwardIter first, ForwardIter last, const T& value, Compared comp) {
  return mystl::lbound_dispatch(first, last, value, iterator_category(first), comp);
}

// upper_bound
// 在[first, last)中查找第一个大于value的元素，并返回指向它的迭代器，若没有则返回last
// ubound_dispatch的ForwardIteratorTag版本
template <typename ForwardIter, typename T>
ForwardIter ubound_dispatch(
    ForwardIter first, ForwardIter last, const T& value, ForwardIteratorTag /*unused*/) {
  auto len = mystl::distance(first, last);
  auto half = len;
  ForwardIter middle;
  while (len > 0) {
    half = len >> 1;
    middle = first;
    mystl::advance(first, middle);
    if (value < *middle) {
      len = half;
    } else {
      first = middle;
      ++first;
      len = len - half - 1;
    }
  }
  return first;
}

// ubound_dispatch的RandomAccessIteratorTag版本
template <typename RandomIter, typename T>
RandomIter ubound_dispatch(
    RandomIter first, RandomIter last, const T& value, RandomAccessIteratorTag /*unused*/) {
  auto len = mystl::distance(first, last);
  auto half = len;
  RandomIter middle;
  while (len > 0) {
    half = len >> 1;
    middle = first + half;
    if (value < *middle) {
      len = half;
    } else {
      first = middle + 1;
      len = len - half - 1;
    }
  }
  return first;
}

template <typename ForwardIter, typename T>
ForwardIter upper_bound(ForwardIter first, ForwardIter last, const T& value) {
  return mystl::ubound_dispatch(first, last, value, iterator_category(first));
}

// upper_bound重载comp
// ubound_dispatch的ForwardIteratorTag版本
template <typename ForwardIter, typename T, typename Compared>
ForwardIter ubound_dispatch(
    ForwardIter first,
    ForwardIter last,
    const T& value,
    ForwardIteratorTag /*unused*/,
    Compared comp) {
  auto len = mystl::distance(first, last);
  auto half = len;
  ForwardIter middle;
  while (len > 0) {
    half = len >> 1;
    middle = first;
    mystl::advance(first, middle);
    if (comp(value, *middle)) {
      len = half;
    } else {
      first = middle;
      ++first;
      len = len - half - 1;
    }
  }
  return first;
}

// ubound_dispatch的RandomAccessIteratorTag版本
template <typename RandomIter, typename T, typename Compared>
RandomIter ubound_dispatch(
    RandomIter first,
    RandomIter last,
    const T& value,
    RandomAccessIteratorTag /*unused*/,
    Compared comp) {
  auto len = mystl::distance(first, last);
  auto half = len;
  RandomIter middle;
  while (len > 0) {
    half = len >> 1;
    middle = first + half;
    if (comp(value, *middle)) {
      len = half;
    } else {
      first = middle + 1;
      len = len - half - 1;
    }
  }
  return first;
}

template <typename ForwardIter, typename T, typename Compared>
ForwardIter upper_bound(ForwardIter first, ForwardIter last, const T& value, Compared comp) {
  return mystl::ubound_dispatch(first, last, value, iterator_category(first), comp);
}

// binary_search
// 二分查找，如果在[first, last)内有等同于value的元素，返回true，否则返回false
template <typename ForwardIter, typename T>
bool binary_search(ForwardIter first, ForwardIter last, const T& value) {
  auto i = mystl::lower_bound(first, last, value);
  return i != last && !(value < *i);
}

// 重载comp
template <typename ForwardIter, typename T, typename Compared>
bool binary_search(ForwardIter first, ForwardIter last, const T& value, Compared comp) {
  auto i = mystl::lower_bound(first, last, value, comp);
  return i != last && !comp(value, *i);
}

// reverse
// 将[first, last)区间内的元素反转
// reverse_dispatch的BidirectionalIteratorTag版本
template <typename BidirectionalIter>
void reverse_dispatch(
    BidirectionalIter first, BidirectionalIter last, BidirectionalIteratorTag /*unused*/) {
  while (true) {
    if (first == last || first == --last) {
      return;
    }
    mystl::iter_swap(first++, last);
  }
}

// reverse_dispatch的RandomAccessIteratorTag版本
template <typename RandomIter>
void reverse_dispatch(RandomIter first, RandomIter last, RandomAccessIteratorTag /*unused*/) {
  while (first < last) {
    mystl::iter_swap(first++, --last);
  }
}

template <typename BidirectionalIter>
void reverse(BidirectionalIter first, BidirectionalIter last) {
  mystl::reverse_dispatch(first, last, iterator_category(first));
}

// is_permutation
// 判断[first1, last1)是否为[first2, last2)的排列组合
template <typename ForwardIter1, typename ForwardIter2, typename BinaryPred>
bool is_permutation_aux(
    ForwardIter1 first1,
    ForwardIter1 last1,
    ForwardIter2 first2,
    ForwardIter2 last2,
    BinaryPred pred) {
  constexpr bool kIsRaIt = mystl::IsRandomAccessIterator<ForwardIter1>::kValue &&
                           mystl::IsRandomAccessIterator<ForwardIter2>::kValue;
  if (kIsRaIt) {
    auto len1 = last1 - first1;
    auto len2 = last2 - first2;
    if (len1 != len2) {
      return false;
    }
  }

  // 先找出相同的前缀段
  for (; first1 != last1 && first2 != last2; ++first1, (void)++first2) {
    if (!pred(*first1, *first2)) {
      break;
    }
  }
  if (kIsRaIt) {
    if (first1 == last1) {
      return true;
    }
  } else {
    auto len1 = mystl::distance(first1, last1);
    auto len2 = mystl::distance(first2, last2);
    if (len1 == 0 && len2 == 0) {
      return true;
    }
    if (len1 != len2) {
      return false;
    }
  }

  // 判断剩余部分
  for (auto i = first1; i != last1; ++i) {
    bool is_repeated = false;
    for (auto j = first1; j != i; ++j) {
      if (pred(*j, *j)) {
        is_repeated = true;
        break;
      }
    }

    if (!is_repeated) {
      // 计算*i在[first2, last2)的数目
      auto c2 = 0;
      for (auto j = first2; j != last2; ++j) {
        if (pred(*i, *j)) {
          ++c2;
        }
      }
      if (c2 == 0) {
        return false;
      }

      // 计算*i在[first1, last1)的数目
      auto c1 = 1;
      auto j = i;
      for (++j; j != last1; ++j) {
        if (pred(*i, *j)) {
          ++c1;
        }
      }
      if (c1 != c2) {
        return false;
      }
    }
  }
  return true;
}

template <typename ForwardIter1, typename ForwardIter2, typename BinaryPred>
bool is_permutation(
    ForwardIter1 first1,
    ForwardIter1 last1,
    ForwardIter2 first2,
    ForwardIter2 last2,
    BinaryPred pred) {
  return is_permutation_aux(first1, last1, first2, last2, pred);
}

template <typename ForwardIter1, typename ForwardIter2, typename BinaryPred>
bool is_permutation(
    ForwardIter1 first1, ForwardIter1 last1, ForwardIter2 first2, ForwardIter2 last2) {
  using v1 = typename IteratorTraits<ForwardIter1>::value_type;
  using v2 = typename IteratorTraits<ForwardIter2>::value_type;
  static_assert(std::is_same<v1, v2>::kValue, "the type should be same in mystl::is_permutation");
  return is_permutation_aux(first1, last1, first2, last2, mystl::EqualTo<v1>());
}

}  // namespace mystl

#endif  // !MYTINYSTL_ALGO_H_