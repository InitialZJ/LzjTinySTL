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
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using value_pointer = T*;
  using map_pointer = T**;

  static const size_type kBufferSize = DequeBufSize<T>::kValue;

  // 迭代器所含成员数据
  value_pointer cur;    // 指向所在缓冲区的当前元素
  value_pointer first;  // 指向所在缓冲区的头部
  value_pointer last;   // 指向所在缓冲区的尾部
  map_pointer node;     // 缓冲区所在结点

  // 构造、复制、移动函数
  DequeIterator() noexcept : cur(nullptr), first(nullptr), last(nullptr), node(nullptr) {}

  DequeIterator(value_pointer v, map_pointer n)
      : cur(v), first(*n), last(*n + kBufferSize), node(n) {}

  DequeIterator(const iterator& rhs)
      : cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {}

  DequeIterator(iterator&& rhs) noexcept
      : cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {
    rhs.cur = nullptr;
    rhs.first = nullptr;
    rhs.last = nullptr;
    rhs.node = nullptr;
  }

  DequeIterator(const const_iterator& rhs)
      : cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {}

  self& operator=(const iterator& rhs) {
    if (this != &rhs) {
      cur = rhs.cur;
      first = rhs.first;
      last = rhs.last;
      node = rhs.node;
    }
    return *this;
  }

  // 转到另一个缓冲区
  void set_node(map_pointer new_node) {
    node = new_node;
    first = *new_node;
    last = first + kBufferSize;
  }

  // 重载运算符
  reference operator*() const { return *cur; }
  pointer operator->() const { return cur; }

  difference_type operator-(const self& x) const {
    return static_cast<difference_type>(kBufferSize) * (node - x.node) + (cur - first) -
           (x.cur - x.first);
  }

  self& operator++() {
    ++cur;
    if (cur == last) {
      set_node(node + 1);
      cur = first;
    }
    return *this;
  }

  self operator++(int) {
    self tmp = *this;
    ++*this;
    return tmp;
  }

  self& operator--() {
    if (cur == first) {
      set_node(node - 1);
      cur = last;
    }
    --cur;
    return *this;
  }

  self operator--(int) {
    self tmp = *this;
    --*this;
    return tmp;
  }
};

}  // namespace mystl

#endif  // ! MYTINYSTL_DEQUE_H_