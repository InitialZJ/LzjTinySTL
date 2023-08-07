#ifndef MYTINYSTL_LIST_H_
#define MYTINYSTL_LIST_H_

// list：双向链表
// 对应书4.3节

// notes:
//
// 异常保证：
// mystl::list<T> 满足基本异常保证，部分函数无异常保证，并对以下等函数做强异常安全保证：
//   * emplace_front
//   * emplace_back
//   * emplace
//   * push_front
//   * push_back
//   * insert

#include <initializer_list>

#include "iterator.h"
#include "memory.h"
// #include "functional.h"
#include "exceptdef.h"
#include "util.h"

namespace mystl {
template <typename T>
struct ListNodeBase;

template <typename T>
struct ListNode;

template <typename T>
struct NodeTraits {
  using base_ptr = ListNodeBase<T>*;
  using node_ptr = ListNode<T>*;
};

// list的节点结构
template <typename T>
struct ListNodeBase {
  using base_ptr = typename NodeTraits<T>::base_ptr;
  using node_ptr = typename NodeTraits<T>::node_ptr;

  base_ptr prev;
  base_ptr next;

  ListNodeBase() = default;

  base_ptr self() { return static_cast<base_ptr>(&*this); }

  node_ptr as_node() { return static_cast<node_ptr>(self()); }

  void unlink() { prev = next = self(); }
};

template <typename T>
struct ListNode : public ListNodeBase<T> {
  using base_ptr = typename NodeTraits<T>::base_ptr;
  using node_ptr = typename NodeTraits<T>::node_ptr;

  T value;

  ListNode() = default;
  explicit ListNode(const T& v) : value(v) {}
  explicit ListNode(T&& v) : value(mystl::move(v)) {}

  base_ptr as_base() { return static_cast<base_ptr>(&*this); }

  node_ptr self() { return static_cast<node_ptr>(&*this); }
};

// 迭代器
template <typename T>
struct ListIterator : public mystl::Iterator<mystl::BidirectionalIteratorTag, T> {
  using value_type = T;
  using pointer = T*;
  using reference = T&;
  using base_ptr = typename NodeTraits<T>::base_ptr;
  using node_ptr = typename NodeTraits<T>::node_ptr;
  using self = ListIterator<T>;

  base_ptr node_;  // 指向当前结点

  ListIterator() = default;
  explicit ListIterator(base_ptr x) : node_(x) {}
  explicit ListIterator(node_ptr x) : node_(x->as_base()) {}
  ListIterator(const ListIterator& rhs) : node_(rhs.node_) {}
};

}  // namespace mystl

#endif  // !MYTINYSTL_LIST_H_