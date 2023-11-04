#ifndef MYTINYSTL_NUMERIC_H_
#define MYTINYSTL_NUMERIC_H_

// 这个头文件包含了 mystl 的数值算法
// 对应书6.3节

#include "iterator.h"

namespace mystl {

// accumulate
// 版本1：以初值init对每个元素进行累加
// 版本2：以处置init堆每个元素进行二元操作

// 版本1
template <typename InputIter, typename T>
T accumulate(InputIter first, InputIter last, T init) {
  for (; first != last; ++first) {
    init += *first;
  }
  return init;
}

// 版本2
template <typename InputIter, typename T, typename BinaryOp>
T accumulate(InputIter first, InputIter last, T init, BinaryOp binary_op) {
  for (; first != last; ++first) {
    init = binary_op(init, *first);
  }
  return init;
}

// adjacent_difference
// 版本1：计算相邻元素的差值，结果保存到以result为起始的区间上
// 版本2：自定义相邻元素的二元操作

// 版本1
template <typename InputIter, typename OutputIter>
OutputIter adjacent_difference(InputIter first, InputIter last, OutputIter result) {
  if (first == last) {
    return result;
  }
  *result = *first;
  auto value = *first;
  while (++first != last) {
    auto tmp = *first;
    *++result = tmp - value;
    value = tmp;
  }
  return ++result;
}

// 版本2
template <typename InputIter, typename OutputIter, typename BinaryOp>
OutputIter adjacent_difference(
    InputIter first, InputIter last, OutputIter result, BinaryOp binary_op) {
  if (first == last) {
    return result;
  }
  *result = *first;
  auto value = *first;
  while (++first != last) {
    auto tmp = *first;
    *++result = binary_op(tmp, value);
    value = tmp;
  }
  return ++result;
}

// inner_product
// 版本1：以init为初值，计算两个区间的内积
// 版本2：自定义operator+和operator*

// 版本1
template <typename InputIter1, typename InputIter2, typename T>
T inner_product(InputIter1 first1, InputIter1 last1, InputIter2 first2, T init) {
  for (; first1 != last1; ++first1, ++first2) {
    init = init + (*first1 * *first2);
  }
  return init;
}

// 版本2
template <
    typename InputIter1,
    typename InputIter2,
    typename T,
    typename BinaryOp1,
    typename BinaryOp2>
T inner_product(
    InputIter1 first1,
    InputIter1 last1,
    InputIter2 first2,
    T init,
    BinaryOp1 binary_op1,
    BinaryOp2 binary_op2) {
  for (; first1 != last1; ++first1, ++first2) {
    init = binary_op1(init, binary_op2(*first1, *first2));
  }
  return init;
}

// iota
// 填充[first, last)，以value为初值开始递增
template <typename ForwardIter, typename T>
void iota(ForwardIter first, ForwardIter last, T value) {
  while (first != last) {
    *first++ = value;
    ++value;
  }
}

// partial_sum
// 版本1：进行局部累计求和，结果保存到以result为起始的区间上
// 版本2：进行局部自定义二元操作

// 版本1
template <typename InputIter, typename OutputIter>
OutputIter partial_sum(InputIter first, InputIter last, OutputIter result) {
  if (first == last) {
    return result;
  }
  *result = *first;
  auto value = *first;
  while (++first != last) {
    value = value + *first;
    *++result = value;
  }
  return ++result;
}

// 版本2
template <typename InputIter, typename OutputIter, typename BinaryOp>
OutputIter partial_sum(InputIter first, InputIter last, OutputIter result, BinaryOp binary_op) {
  if (first == last) {
    return result;
  }
  *result = *first;
  auto value = *first;
  while (++first != last) {
    value = binary_op(value, *first);
    *++result = value;
  }
  return ++result;
}

}  // namespace mystl
#endif  // !MYTINYSTL_NUMERIC_H_
