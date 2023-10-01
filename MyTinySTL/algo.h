#ifndef MYTINYSTL_ALGO_H_
#define MYTINYSTL_ALGO_H_

#include "iterator.h"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)
#endif

// 包含一系列算法

#include <cstddef>
#include <ctime>

#include "algobase.h"
#include "functional.h"
#include "heap_algo.h"
#include "memory.h"

namespace mystl {

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