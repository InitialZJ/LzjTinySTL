#ifndef MYTINYSTL_HEAP_ALGO_H_
#define MYTINYSTL_HEAP_ALGO_H_

// 包含 heap 的四个算法 : push_heap, pop_heap, sort_heap, make_heap
// 对应书4.7节

#include "iterator.h"

namespace mystl {

// push_heap
// 接受两个迭代器，表示heap容器的首尾，并且新元素已经插入到底部容器的最尾端，调整heap
template <typename RandomIter, typename Distance, typename T>
void push_heap_aux(RandomIter first, Distance holeIndex, Distance topIndex, T value) {
  auto parent = (holeIndex - 1) / 2;
  while (holeIndex > topIndex && *(first + parent) < value) {
    // 使用operator<，所以heap为max_heap
    *(first + holeIndex) = *(first + parent);
    holeIndex = parent;
    parent = (holeIndex - 1) / 2;
  }
  *(first + holeIndex) = value;
}

template <typename RandomIter, typename Distance>
void push_heap_d(RandomIter first, RandomIter last, Distance* /*unused*/) {
  mystl::push_heap_aux(first, (last - first) - 1, static_cast<Distance>(0), *(last - 1));
}

template <typename RandomIter>
void push_heap(RandomIter first, RandomIter last) {
  mystl::push_heap_d(first, last, distance_type(first));
}

// 重载版本使用函数对象comp代替比较操作
template <typename RandomIter, typename Distance, typename T, typename Compared>
void push_heap_aux(
    RandomIter first, Distance holeIndex, Distance topIndex, T value, Compared comp) {
  auto parent = (holeIndex - 1) / 2;
  while (holeIndex > topIndex && comp(*(first + parent), value)) {
    *(first + holeIndex) = *(first + parent);
    holeIndex = parent;
    parent = (holeIndex - 1) / 2;
  }
  *(first + holeIndex) = value;
}

template <typename RandomIter, typename Compared, typename Distance>
void push_heap_d(RandomIter first, RandomIter last, Distance* /*unused*/, Compared comp) {
  mystl::push_heap_aux(first, (last - first) - 1, static_cast<Distance>(0), *(last - 1), comp);
}

template <typename RandomIter, typename Compared>
void push_heap(RandomIter first, RandomIter last, Compared comp) {
  mystl::push_heap_d(first, last, distance_type(first), comp);
}

// pop_heap
// 接受两个迭代器，表示heap容器的首尾，将heap的根节点取出放到容器尾部，调整heap
template <typename RandomIter, typename T, typename Distance>
void adjust_heap(RandomIter first, Distance holeIndex, Distance len, T value) {
  // 先进性下溯(percolate down)过程
  auto topIndex = holeIndex;
  auto rchild = 2 * holeIndex + 2;
  while (rchild < len) {
    if (*(first + rchild) < (*first + rchild - 1)) {
      --rchild;
    }
    *(first + holeIndex) = *(first + rchild);
    holeIndex = rchild;
    rchild = 2 * (rchild + 1);
  }
  if (rchild == len) {
    // 如果没有右子节点
    *(first + holeIndex) = *(first + (rchild - 1));
    holeIndex = rchild - 1;
  }
  // 再执行一次上溯(percolate up)过程
  mystl::push_heap_aux(first, holeIndex, topIndex, value);
}

template <typename RandomIter, typename T, typename Distance>
void pop_heap_aux(
    RandomIter first, RandomIter last, RandomIter result, T value, Distance* /*unused*/) {
  // 先将首值调至尾结点，然后调整[first, last - 1)使之成为一个max-heap
  *result = *first;
  mystl::adjust_heap(first, static_cast<Distance>(0), last - first, value);
}

template <typename RandomIter>
void pop_heap(RandomIter first, RandomIter last) {
  mystl::pop_heap_aux(first, last - 1, last - 1, *(last - 1), distance_type(first));
}

// 重载版本使用函数对象 comp 代替比较操作
template <class RandomIter, class T, class Distance, class Compared>
void adjust_heap(RandomIter first, Distance holeIndex, Distance len, T value, Compared comp) {
  // 先进行下溯(percolate down)过程
  auto topIndex = holeIndex;
  auto rchild = 2 * holeIndex + 2;
  while (rchild < len) {
    if (comp(*(first + rchild), *(first + rchild - 1))) {
      --rchild;
    }
    *(first + holeIndex) = *(first + rchild);
    holeIndex = rchild;
    rchild = 2 * (rchild + 1);
  }
  if (rchild == len) {
    *(first + holeIndex) = *(first + (rchild - 1));
    holeIndex = rchild - 1;
  }
  // 再执行一次上溯(percolate up)过程
  mystl::push_heap_aux(first, holeIndex, topIndex, value, comp);
}

template <class RandomIter, class T, class Distance, class Compared>
void pop_heap_aux(
    RandomIter first,
    RandomIter last,
    RandomIter result,
    T value,
    Distance* /*unused*/,
    Compared comp) {
  *result = *first;  // 先将尾指设置成首值，即尾指为欲求结果
  mystl::adjust_heap(first, static_cast<Distance>(0), last - first, value, comp);
}

template <class RandomIter, class Compared>
void pop_heap(RandomIter first, RandomIter last, Compared comp) {
  mystl::pop_heap_aux(first, last - 1, last - 1, *(last - 1), distance_type(first), comp);
}

// sort_heap
// 接受两个迭代器，表示heap容器的首尾，不断执行pop_heap操作，直到首尾最多相差1
template <typename RandomIter>
void sort_heap(RandomIter first, RandomIter last) {
  // 每执行一次pop_heap，最大的元素都被放到尾部，直到容器最多只有一个元素，完成排序
  while (last - first > 1) {
    mystl::pop_heap(first, last--);
  }
}

// 重载版本使用函数对象 comp 代替比较操作
template <class RandomIter, class Compared>
void sort_heap(RandomIter first, RandomIter last, Compared comp) {
  while (last - first > 1) {
    mystl::pop_heap(first, last--, comp);
  }
}

// make_heap
// 接受两个迭代器，表示heap容器的首尾，把容器内的数据变成一个heap
template <typename RandomIter, typename Distance>
void make_heap_aux(RandomIter first, RandomIter last, Distance* /*unused*/) {
  if (last - first < 2) {
    return;
  }
  auto len = last - first;
  auto holeIndex = (len - 2) / 2;
  while (true) {
    // 重排以holeIndex为首的子树
    mystl::adjust_heap(first, holeIndex, len, *(first + holeIndex));
    if (holeIndex == 0) {
      return;
    }
    holeIndex--;
  }
}

template <typename RandomIter>
void make_heap(RandomIter first, RandomIter last) {
  mystl::make_heap_aux(first, last, distance_type(first));
}
// 重载版本使用函数对象 comp 代替比较操作
template <class RandomIter, class Distance, class Compared>
void make_heap_aux(RandomIter first, RandomIter last, Distance*, Compared comp) {
  if (last - first < 2) return;
  auto len = last - first;
  auto holeIndex = (len - 2) / 2;
  while (true) {
    // 重排以 holeIndex 为首的子树
    mystl::adjust_heap(first, holeIndex, len, *(first + holeIndex), comp);
    if (holeIndex == 0) {
      return;
    }
    holeIndex--;
  }
}

template <class RandomIter, class Compared>
void make_heap(RandomIter first, RandomIter last, Compared comp) {
  mystl::make_heap_aux(first, last, distance_type(first), comp);
}
}  // namespace mystl

#endif  // ! MYTINYSTL_HEAP_ALGO_H_