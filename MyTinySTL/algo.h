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
#include "memory.h"
// #include "heap_algo.h"
// #include "functional.h"

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
  FOrwardIter middle;
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

}  // namespace mystl

#endif  // !MYTINYSTL_ALGO_H_