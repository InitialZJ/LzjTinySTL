#ifndef MYTINYSTL_STACK_H_
#define MYTINYSTL_STACK_H_

// stack：栈
// 对应书4.5节

#include "deque.h"

namespace mystl {
// 模板类Stack
// 参数一代表数据类型，参数二代表底层容器类型，缺省使用mystl::deque作为底层容器
template <typename T, typename Container = mystl::Deque<T>>
class Stack {
 public:
  using container_type = Container;
  using value_type = typename Container::value_type;
  using size_type = typename Container::size_type;
  using reference = typename Container::reference;
  using const_reference = typename Container::const_reference;

  static_assert(
      std::is_same<T, value_type>::value, "thie value_type of Container should be same with T");

 private:
  container_type c_;  // 用底层容器表现stack

 public:
  // 构造、复制、移动函数
  Stack() = default;

  explicit Stack(size_type n) : c_(n) {}

  Stack(size_type n, const value_type& value) : c_(n, value) {}

  template <typename IIter>
  Stack(IIter first, IIter last) : c_(first, last) {}

  Stack(std::initializer_list<T> ilist) : c_(ilist.begin(), ilist.end()) {}

  Stack(const Container& c) : c_(c) {}

  Stack(Container&& c) noexcept(std::is_nothrow_move_constructible<Container>::value)
      : c_(mystl::move(c)) {}

  Stack(const Stack& rhs) : c_(rhs.c_) {}

  Stack& operator=(const Stack& rhs) {
    c_ = rhs.c_;
    return *this;
  }

  Stack& operator=(Stack&& rhs) noexcept(std::is_nothrow_move_assignable<Container>::value) {
    c_ = mystl::move(rhs.c_);
    return *this;
  }

  Stack& operator=(std::initializer_list<T> ilist) {
    c_ = ilist;
    return *this;
  }

  ~Stack() = default;

  // 访问元素相关操作
  reference top() { return c_.back(); }
  const_reference top() const { return c_.back(); }

  // 容量相关操作
  bool empty() const noexcept { return c_.empty(); }
  size_type size() const noexcept { return c_.size(); }

  // 修改容器相关操作
  template <typename... Args>
  void emplace(Args&&... args) {
    c_.emplace_back(mystl::forward<Args>(args)...);
  }

  void push(const value_type& value) { c_.push_back(value); }
  void push(value_type&& value) { c_.push_back(mystl::move(value)); }

  void pop() { c_.pop_back(); }

  void clear() {
    while (!empty()) {
      pop();
    }
  }

  void swap(Stack& rhs) noexcept(noexcept(mystl::swap(c_, rhs.c_))) { mystl::swap(c_, rhs.c_); }

 public:
  friend bool operator==(const Stack& lhs, const Stack& rhs) { return lhs.c_ == rhs.c_; }
  friend bool operator<(const Stack& lhs, const Stack& rhs) { return lhs.c_ < rhs.c_; }
};

// 重载比较操作符
template <typename T, typename Container>
bool operator==(const Stack<T, Container>& lhs, const Stack<T, Container>& rhs) {
  return lhs == rhs;
}

template <typename T, typename Container>
bool operator<(const Stack<T, Container>& lhs, const Stack<T, Container>& rhs) {
  return lhs < rhs;
}

template <typename T, typename Container>
bool operator!=(const Stack<T, Container>& lhs, const Stack<T, Container>& rhs) {
  return (lhs == rhs);
}

template <typename T, typename Container>
bool operator>(const Stack<T, Container>& lhs, const Stack<T, Container>& rhs) {
  return rhs < lhs;
}

template <typename T, typename Container>
bool operator<=(const Stack<T, Container>& lhs, const Stack<T, Container>& rhs) {
  return !(rhs < lhs);
}

template <typename T, typename Container>
bool operator>=(const Stack<T, Container>& lhs, const Stack<T, Container>& rhs) {
  return !(lhs < rhs);
}

template <typename T, typename Container>
void swap(Stack<T, Container>& lhs, Stack<T, Container>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
  lhs.swap(rhs);
}

}  // namespace mystl

#endif  // ! MYTINYSTL_STACK_H_
