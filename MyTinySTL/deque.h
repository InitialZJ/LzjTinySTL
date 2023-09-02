#ifndef MYTINYSTL_DEQUE_H_
#define MYTINYSTL_DEQUE_H_

// deque：双端队列

// notes:
//
// 异常保证：
// mystl::deque<T> 满足基本异常保证，部分函数无异常保证，并对以下等函数做强异常安全保证：
//   * emplace_front
//   * emplace_back
//   * emplace
//   * push_front
//   * push_back
//   * insert

#include <initializer_list>

#include "exceptdef.h"
#include "iterator.h"
#include "memory.h"
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

// deque map初始化的大小
#ifndef DEQUE_MAP_INIT_SIZE
#define DEQUE_MAP_INIT_SIZE 8
#endif

template <typename T>
struct DequeBufSize {
  static constexpr size_t kValue = sizeof(T) < 256 ? 4096 / sizeof(T) : 16;
};

// deque迭代器设计
template <typename T, typename Ref, typename Ptr>
struct DequeIterator : public Iterator<RandomAccessIteratorTag, T> {
  using iterator = DequeIterator<T, T&, T*>;
  using const_iterator = DequeIterator<T, const T&, const T*>;
  using self = DequeIterator;

  using value_type = T;
  using pointer = Ptr;
  using reference = Ref;
};

}  // namespace mystl

#endif  // ! MYTINYSTL_DEQUE_H_