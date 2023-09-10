#ifndef MYTINYSTL_QUEUE_H_
#define MYTINYSTL_QUEUE_H_

// queue          : 队列
// 对应书4.6节

// priority_queue : 优先队列
// 对应书4.8节

#include <initializer_list>
#include <type_traits>

#include "deque.h"
#include "functional.h"
#include "heap_algo.h"
#include "vector.h"

namespace mystl {

// 模板类Queue
// 参数一代表数据类型，菜蔬二代表底层容器类型，缺省使用mystl::Deque作为底层容器
template <typename T, typename Container = mystl::Deque<T>>
class Queue {
 public:
  using container_type = Container;
  using value_type = typename Container::value_type;
  using size_type = typename Container::size_type;
  using reference = typename Container::reference;
  using const_reference = typename Container::const_reference;

  static_assert(
      std::is_same<T, value_type>::value, "the value_type of Container should be same with T");

 private:
  container_type c_;  // 用底层容器表现Queue

 public:
  // 构造、复制、移动函数
  Queue() = default;

  explicit Queue(size_type n) : c_(n) {}

  Queue(size_type n, const value_type& value) : c_(n, value) {}

  template <typename IIter>
  Queue(IIter first, IIter last) : c_(first, last) {}

  Queue(std::initializer_list<T> ilist) : c_(ilist.begin(), ilist.end()) {}

  Queue(const Container& c) : c_(c) {}

  Queue(Container&& c) noexcept(std::is_nothrow_move_constructible<Container>::value)
      : c_(mystl::move(c)) {}

  Queue(const Queue& rhs) : c_(rhs.c_) {}

  Queue(Queue&& rhs) noexcept(std::is_nothrow_move_constructible<Container>::value)
      : c_(mystl::move(rhs.c_)) {}

  Queue& operator=(const Queue& rhs) {
    c_ = rhs.c_;
    return *this;
  }

  Queue& operator=(Queue&& rhs) noexcept(std::is_nothrow_move_assignable<Container>::value) {
    c_ = mystl::move(rhs.c_);
    return *this;
  }

  Queue& operator=(std::initializer_list<T> ilist) {
    c_ = ilist;
    return *this;
  }

  ~Queue() = default;

  // 访问元素相关操作
  reference front() { return c_.front(); }
  const_reference front() const { return c_.front(); }
  reference back() { return c_.back(); }
  const_reference back() const { return c_.back(); }

  // 容量相关操作
  bool empty() const noexcept { return c_.empty(); }
  size_type size() const noexcept { return c_.size(); }

  // 修改容器相关操作
  template <typename... Args>
  void emplace(Args&&... args) {
    c_.emplace_back(mystl::forward<Args>(args)...);
  }

  void push(const value_type& value) { c_.push_back(value); }

  void push(value_type&& value) { c_.emplace_back(mystl::move(value)); }

  void pop() { c_.pop_front(); }

  void clear() {
    while (!empty()) {
      pop();
    }
  }

  void swap(Queue& rhs) noexcept(noexcept(mystl::swap(c_, rhs.c_))) { mystl::swap(c_, rhs.c_); }

 public:
  friend bool operator==(const Queue& lhs, const Queue& rhs) { return lhs.c_ == rhs.c_; }
  friend bool operator<(const Queue& lhs, const Queue& rhs) { return lhs.c_ < rhs.c_; }
};

// 重载比较操作符
template <typename T, typename Container>
bool operator==(const Queue<T, Container>& lhs, const Queue<T, Container>& rhs) {
  return lhs == rhs;
}

// 重载比较操作符
template <typename T, typename Container>
bool operator!=(const Queue<T, Container>& lhs, const Queue<T, Container>& rhs) {
  return !(lhs == rhs);
}

// 重载比较操作符
template <typename T, typename Container>
bool operator<(const Queue<T, Container>& lhs, const Queue<T, Container>& rhs) {
  return lhs < rhs;
}

// 重载比较操作符
template <typename T, typename Container>
bool operator>(const Queue<T, Container>& lhs, const Queue<T, Container>& rhs) {
  return !(rhs < lhs);
}

// 重载比较操作符
template <typename T, typename Container>
bool operator<=(const Queue<T, Container>& lhs, const Queue<T, Container>& rhs) {
  return !(rhs < lhs);
}

// 重载比较操作符
template <typename T, typename Container>
bool operator>=(const Queue<T, Container>& lhs, const Queue<T, Container>& rhs) {
  return !(lhs < rhs);
}

// 重载mystl的swap
template <typename T, typename Container>
void swap(Queue<T, Container>& lhs, Queue<T, Container>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
  lhs.swap(rhs);
}

// 模板类 priority_queue
// 参数一代表数据类型，参数二代表容器类型，缺省使用mystl::Vector作为底层容器
// 参数三代表比较权值的方式，缺省使用mystl::Less作为比较方式
template <
    typename T,
    typename Container = mystl::Vector<T>,
    typename Compare = mystl::Less<typename Container::value_type>>
class PriorityQueue {
 public:
  using container_type = Container;
  using value_compare = Compare;
  using value_type = typename Container::value_type;
  using size_type = typename Container::size_type;
  using reference = typename Container::reference;
  using const_reference = typename Container::const_reference;

  static_assert(
      std::is_same<T, value_type>::value, "the value_type of Container should be same with T");

 private:
  container_type c_;
  value_compare comp_;

 public:
  // 构造、复制、移动函数
  PriorityQueue() = default;

  PriorityQueue(const Compare& c) : c_(), comp_(c) {}

  explicit PriorityQueue(size_type n) : c_(n) { mystl::make_heap(c_.begin(), c_.end(), comp_); }

  PriorityQueue(size_type n, const value_type& value) : c_(n, value) {
    mystl::make_heap(c_.begin(), c_.end(), comp_);
  }

  template <typename IIter>
  PriorityQueue(IIter first, IIter last) : c_(first, last) {
    mystl::make_heap(c_.begin(), c_.end(), comp_);
  }

  PriorityQueue(std::initializer_list<T> ilist) : c_(ilist) {
    mystl::make_heap(c_.begin(), c_.end(), comp_);
  }

  PriorityQueue(const Container& s) : c_(s) { mystl::make_heap(c_.begin(), c_.end(), comp_); }

  PriorityQueue(Container&& s) : c_(mystl::move(s)) {
    mystl::make_heap(c_.begin(), c_.end(), comp_);
  }

  PriorityQueue(const PriorityQueue& rhs) : c_(rhs.c_), comp_(rhs.comp_) {
    mystl::make_heap(c_.begin(), c_.end(), comp_);
  }

  PriorityQueue(PriorityQueue&& rhs) : c_(mystl::move(rhs.c_)), comp_(rhs.comp_) {
    mystl::make_heap(c_.begin(), c_.end(), comp_);
  }

  PriorityQueue& operator=(const PriorityQueue& rhs) {
    c_ = rhs.c_;
    comp_ = rhs.comp_;
    mystl::make_heap(c_.begin(), c_.end(), comp_);
    return *this;
  }

  PriorityQueue& operator=(PriorityQueue&& rhs) {
    c_ = mystl::move(rhs.c_);
    comp_ = rhs.comp_;
    mystl::make_heap(c_.begin(), c_.end(), comp_);
    return *this;
  }

  PriorityQueue& operator=(std::initializer_list<T> ilist) {
    c_ = ilist;
    comp_ = value_compare();
    mystl::make_heap(c_.begin(), c_.end(), comp_);
    return *this;
  }

  ~PriorityQueue() = default;

 public:
  // 访问元素相关操作
  const_reference top() const { return c_.front(); }

  // 容量相关操作
  bool empty() const noexcept { return c_.empty(); }
  size_type size() const noexcept { return c_.size(); }

  // 修改容器相关操作
  template <typename... Args>
  void emplace(Args&&... args) {
    c_.emplace_back(mystl::forward<Args>(args)...);
    mystl::push_heap(c_.begin(), c_.end(), comp_);
  }

  void push(const value_type& value) {
    c_.push_back(value);
    mystl::push_heap(c_.begin(), c_.end(), comp_);
  }

  void push(value_type&& value) {
    c_.push_back(mystl::move(value));
    mystl::push_heap(c_.begin(), c_.end(), comp_);
  }

  void pop() {
    mystl::pop_heap(c_.begin(), c_.end(), comp_);
    c_.pop_back();
  }

  void clear() {
    while (!empty()) {
      pop();
    }
  }

  void swap(PriorityQueue& rhs) noexcept(
      noexcept(mystl::swap(c_, rhs.c_)) && noexcept(mystl::swap(comp_, rhs.comp_))) {
    mystl::swap(c_, rhs.c_);
    mystl::swap(comp_, rhs.comp_);
  }

 public:
  friend bool operator==(const PriorityQueue& lhs, const PriorityQueue& rhs) {
    return lhs.c_ == rhs.c_;
  }

  friend bool operator!=(const PriorityQueue& lhs, const PriorityQueue& rhs) {
    return !(lhs.c_ == rhs.c_);
  }
};

// 重载比较操作符
template <typename T, typename Container, typename Compare>
bool operator==(
    PriorityQueue<T, Container, Compare>& lhs, PriorityQueue<T, Container, Compare>& rhs) {
  return lhs == rhs;
}

template <typename T, typename Container, typename Compare>
bool operator!=(
    PriorityQueue<T, Container, Compare>& lhs, PriorityQueue<T, Container, Compare>& rhs) {
  return !(lhs == rhs);
}

template <typename T, typename Container, typename Compare>
void swap(
    PriorityQueue<T, Container, Compare>& lhs,
    PriorityQueue<T, Container, Compare>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
  lhs.swap(rhs);
}

}  // namespace mystl

#endif  // ! MYTINYSTL_QUEUE_H_
