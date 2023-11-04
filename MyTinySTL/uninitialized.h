#ifndef MYTINYSTL_UNINITIALIZED_H_
#define MYTINYSTL_UNINITIALIZED_H_

// 用于对未初始化空间构造元素
// 对应书2.3节

#include "algobase.h"
#include "construct.h"
// #include "iterator.h"

namespace mystl {

// 把[first, last)上的内容复制到以result为起始处的空间，返回复制结束的位置
template <typename InputIter, typename ForwardIter>
ForwardIter unchecked_uninit_copy(
    InputIter first, InputIter last, ForwardIter result, std::true_type /*unused*/) {
  return mystl::copy(first, last, result);
}

template <typename InputIter, typename ForwardIter>
ForwardIter unchecked_uninit_copy(
    InputIter first, InputIter last, ForwardIter result, std::false_type /*unused*/) {
  auto cur = result;
  try {
    for (; first != last; ++first, ++cur) {
      mystl::construct(&*cur, *first);
    }
  } catch (...) {
    for (; result != cur; --cur) {
      mystl::destroy(&*cur);
    }
  }
  return cur;
}

template <typename InputIter, typename ForwardIter>
ForwardIter uninitialized_copy(InputIter first, InputIter last, ForwardIter result) {
  return mystl::unchecked_uninit_copy(
      first,
      last,
      result,
      std::is_trivially_copy_assignable<typename IteratorTraits<ForwardIter>::value_type>{});
}

// 把[first, first + n)上的内容复制到以result为起始处的空间，返回复制结束的位置
template <typename InputIter, typename Size, typename ForwardIter>
ForwardIter unchecked_uninit_copy_n(
    InputIter first, Size n, ForwardIter result, std::true_type /*unused*/) {
  return mystl::copy_n(first, n, result).second;
}

template <typename InputIter, typename Size, typename ForwardIter>
ForwardIter unchecked_uninit_copy_n(
    InputIter first, Size n, ForwardIter result, std::false_type /*unused*/) {
  auto cur = result;
  try {
    for (; n > 0; --n, ++cur, ++first) {
      mystl::construct(&*cur, *first);
    }
  } catch (...) {
    for (; result != cur; --cur) {
      mystl::destroy(&*cur);
    }
  }
  return cur;
}

template <typename InputIter, typename Size, typename ForwardIter>
ForwardIter uninitialized_copy_n(InputIter first, Size n, ForwardIter result) {
  return mystl::unchecked_uninit_copy_n(
      first,
      n,
      result,
      std::is_trivially_copy_assignable<typename IteratorTraits<InputIter>::value_type>{});
}

// 在[first, last)区间内填充元素值
template <typename ForwardIter, typename T>
void unchecked_uninit_fill(
    ForwardIter first, ForwardIter last, const T& value, std::true_type /*unused*/) {
  mystl::fill(first, last, value);
}

template <typename ForwardIter, typename T>
void unchecked_uninit_fill(
    ForwardIter first, ForwardIter last, const T& value, std::false_type /*unused*/) {
  auto cur = first;
  try {
    for (; cur != last; ++cur) {
      mystl::construct(&*cur, value);
    }
  } catch (...) {
    for (; first != cur; ++first) {
      mystl::destroy(&*first);
    }
  }
}

template <typename ForwardIter, typename T>
void uninitialized_fill(ForwardIter first, ForwardIter last, const T& value) {
  mystl::unchecked_uninit_fill(
      first,
      last,
      value,
      std::is_trivially_copy_assignable<typename IteratorTraits<ForwardIter>::value_type>{});
}

// 从first位置开始，填充n个元素值，返回填充结束的位置
template <typename ForwardIter, typename Size, typename T>
ForwardIter unchecked_uninit_fill_n(
    ForwardIter first, Size n, const T& value, std::true_type /*unused*/) {
  return mystl::fill_n(first, n, value);
}

template <typename ForwardIter, typename Size, typename T>
ForwardIter unchecked_uninit_fill_n(
    ForwardIter first, Size n, const T& value, std::false_type /*unused*/) {
  auto cur = first;
  try {
    for (; n > 0; --n, ++cur) {
      mystl::construct(&*cur, value);
    }
  } catch (...) {
    for (; first != cur; ++first) {
      mystl::destroy(&*first);
    }
  }
  return cur;
}

template <typename ForwardIter, typename Size, typename T>
ForwardIter uninitialized_fill_n(ForwardIter first, Size n, const T& value) {
  return mystl::unchecked_uninit_fill_n(
      first,
      n,
      value,
      std::is_trivially_copy_assignable<typename IteratorTraits<ForwardIter>::value_type>{});
}

// 把[first, last)上的内容移动到以result为起始处的空间，返回移动结束的位置
template <typename InputIter, typename ForwardIter>
ForwardIter unchecked_uninit_move(
    InputIter first, InputIter last, ForwardIter result, std::true_type /*unused*/) {
  return mystl::move(first, last, result);
}

template <typename InputIter, typename ForwardIter>
ForwardIter unchecked_uninit_move(
    InputIter first, InputIter last, ForwardIter result, std::false_type /*unused*/) {
  ForwardIter cur = first;
  try {
    for (; first != last; ++first, ++cur) {
      mystl::construct(&*cur, mystl::move(*first));
    }
  } catch (...) {
    mystl::destroy(result, cur);
  }
  return cur;
}

template <typename InputIter, typename ForwardIter>
ForwardIter uninitialized_move(InputIter first, InputIter last, ForwardIter result) {
  return mystl::unchecked_uninit_move(
      first,
      last,
      result,
      std::is_trivially_move_assignable<typename IteratorTraits<InputIter>::value_type>{});
}

// uninitialized_move_n
// 把[first, first + n)上的内容移动到以result为初始处的空间，返回移动结束的位置
template <typename InputIter, typename Size, typename ForwardIter>
ForwardIter unchecked_uninit_move_n(
    InputIter first, Size n, ForwardIter result, std::true_type /*unused*/) {
  return mystl::move(first, first + n, result);
}

template <typename InputIter, typename Size, typename ForwardIter>
ForwardIter unchecked_uninit_move_n(
    InputIter first, Size n, ForwardIter result, std::false_type /*unused*/) {
  auto cur = result;
  try {
    for (; n > 0; --n, ++first, ++cur) {
      mystl::construct(&*cur, mystl::move(*first));
    }
  } catch (...) {
    for (; result != cur; ++result) {
      mystl::destroy(&*result);
    }
    throw;
  }
  return cur;
}

template <typename InputIter, typename Size, typename ForwardIter>
ForwardIter uninitialized_move_n(InputIter first, Size n, ForwardIter result) {
  return mystl::unchecked_uninit_move_n(
      first,
      n,
      result,
      std::is_trivially_move_assignable<typename IteratorTraits<InputIter>::value_type>{});
}

}  // namespace mystl

#endif  // !MYTINYSTL_UNINITIALIZED_H_