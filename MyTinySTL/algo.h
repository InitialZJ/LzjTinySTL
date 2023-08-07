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

}  // namespace mystl

#endif  // !MYTINYSTL_ALGO_H_