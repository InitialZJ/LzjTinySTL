#ifndef MYTINYSTL_RB_TREE_H_
#define MYTINYSTL_RB_TREE_H_

// 模板类 rb_tree
// rb_tree : 红黑树
// 对应书5.2节

#include <cassert>
#include <initializer_list>

#include "exceptdef.h"
#include "functional.h"
#include "iterator.h"
#include "memory.h"
#include "type_traits.h"

namespace mystl {

// 结点颜色类型
using rb_tree_color_type = bool;

static constexpr rb_tree_color_type kRbTreeRed = false;
static constexpr rb_tree_color_type kRbTreeBlack = true;

// forward declaration
template <typename T>
struct RbTreeNodeBase;

template <typename T>
struct RbTreeNode;

template <typename T>
struct RbTreeIterator;

template <typename T>
struct RbTreeConstIterator;

// rb tree value traits
template <typename T, bool>
struct RbTreeValueTraitsImp {
  using key_type = T;
  using mapped_type = T;
  using value_type = T;

  template <typename Ty>
  static const key_type& get_key(const Ty& value) {
    return value;
  }

  template <typename Ty>
  static const value_type& get_value(const Ty& value) {
    return value;
  }
};

template <typename T>
struct RbTreeValueTraitsImp<T, true> {
  using key_type = typename std::remove_cv<typename T::first_type>::type;
  using mapped_type = typename T::second_type;
  using value_type = T;

  template <typename Ty>
  static const key_type& get_key(const Ty& value) {
    return value.first;
  }

  template <typename Ty>
  static const value_type& get_value(const Ty& value) {
    return value;
  }
};

template <typename T>
struct RbTreeValueTraits {
  static constexpr bool kIsMap = mystl::IsPair<T>::kValue;

  using value_traits_type = RbTreeValueTraitsImp<T, kIsMap>;

  using key_type = typename value_traits_type::key_type;
  using mapped_type = typename value_traits_type::mapped_type;
  using value_type = typename value_traits_type::value_type;

  template <typename Ty>
  static const key_type& get_key(const Ty& value) {
    return value_traits_type::get_key(value);
  }

  template <typename Ty>
  static const value_type& get_value(const Ty& value) {
    return value_traits_type::get_value(value);
  }
};

// rb tree node traits
template <typename T>
struct RbTreeNodeTraits {
  using color_type = rb_tree_color_type;

  using value_traits = RbTreeValueTraits<T>;
  using key_type = typename value_traits::key_type;
  using mapped_type = typename value_traits::mapped_type;
  using value_type = typename value_traits::value_type;

  using base_ptr = RbTreeNodeBase<T>*;
  using node_ptr = RbTreeNode<T>*;
};

// rb tree的结点设计
template <typename T>
struct RbTreeNodeBase {
  using color_type = rb_tree_color_type;
  using base_ptr = RbTreeNodeBase<T>*;
  using node_ptr = RbTreeNode<T>*;

  base_ptr parent;   // 父结点
  base_ptr left;     // 左子结点
  base_ptr right;    // 右子结点
  color_type color;  // 结点颜色

  base_ptr get_base_ptr() { return &*this; }

  node_ptr get_node_ptr() { return reinterpret_cast<node_ptr>(&*this); }

  node_ptr& get_node_ref() { return reinterpret_cast<node_ptr&>(*this); }
};

template <typename T>
struct RbTreeNode : public RbTreeNodeBase<T> {
  using base_ptr = RbTreeNodeBase<T>*;
  using node_ptr = RbTreeNode<T>*;

  T value;

  base_ptr get_base_ptr() { return static_cast<base_ptr>(&*this); }

  node_ptr get_node_ptr() { return &*this; }
};

// rb tree traits
template <typename T>
struct RbTreeTraits {
  using value_traits = RbTreeValueTraits<T>;

  using key_type = typename value_traits::key_type;
  using mapped_type = typename value_traits::mapped_type;
  using value_type = typename value_traits::value_type;

  using pointer = value_type*;
  using reference = value_type&;
  using const_pointer = const value_type*;
  using const_reference = const value_type&;

  using base_type = RbTreeNodeBase<T>;
  using node_type = RbTreeNode<T>;

  using base_ptr = base_type*;
  using node_ptr = node_type*;
};

// rb tree的迭代器设计
template <typename T>
struct RbTreeIteratorBase : public mystl::Iterator<mystl::BidirectionalIteratorTag, T> {
  using base_ptr = typename RbTreeTraits<T>::base_ptr;

  base_ptr node;  // 指向结点本身

  RbTreeIteratorBase() : node(nullptr) {}

  // 使迭代器前进
  void inc() {
    if (node->right != nullptr) {
      node = rb_tree_min(node->right);
    } else {
      // 如果没有右子结点
      auto y = node->parent;
      while (y->right == node) {
        node = y;
        y = y->parent;
      }
      if (node->right != y) {
        // 用于应对“寻找根结点的下一结点，而根结点没有右子结点”的特殊情况
        node = y;
      }
    }
  }

  // 使迭代器后退
  void dec() {
    if (node->parent->parent == node && rb_tree_is_red(node)) {
      // 如果node为header
      node = node->right;  // 指向整棵树的max结点
      // TODO(lzj): why
    } else if (node->left != nullptr) {
      node = rb_tree_max(node->left);
    } else {
      // 非header结点，也无左子结点
      // TODO(lzj): 如果node是最小值呢
      auto y = node->parent;
      while (node == y->left) {
        node = y;
        y = y->parent;
      }
      node = y;
    }
  }

  bool operator==(const RbTreeIteratorBase& rhs) { return node == rhs.node; }
  bool operator!=(const RbTreeIteratorBase& rhs) { return node != rhs.node; }
};

template <typename T>
struct RbTreeIterator : public RbTreeIteratorBase<T> {
  using tree_traits = RbTreeTraits<T>;

  using value_type = typename tree_traits::value_type;
  using pointer = typename tree_traits::pointer;
  using reference = typename tree_traits::reference;
  using base_ptr = typename tree_traits::base_ptr;
  using node_ptr = typename tree_traits::node_ptr;

  using iterator = RbTreeIterator<T>;
  using const_iterator = RbTreeConstIterator<T>;
  using self = iterator;

  using RbTreeIteratorBase<T>::node;

  // 构造函数
  RbTreeIterator() {}
  RbTreeIterator(base_ptr x) { node = x; }
  RbTreeIterator(node_ptr x) { node = x; }
  RbTreeIterator(const iterator& rhs) { node = rhs.node; }
  RbTreeIterator(const const_iterator& rhs) { node = rhs.node; }

  // 重载操作符
  reference operator*() const { return node->get_node_ptr()->value; }
  pointer operator->() const { return &(operator*()); }

  self& operator++() {
    this->inc();
    return *this;
  }

  self operator++(int) {
    self tmp(*this);
    this->inc();
    return tmp;
  }

  self& operator--() {
    this->dec();
    return *this;
  }

  self operator--(int) {
    self tmp(*this);
    this->dec();
    return tmp;
  }
};

template <typename T>
struct RbTreeConstIterator : public RbTreeIteratorBase<T> {
  using tree_traits = RbTreeTraits<T>;

  using value_type = typename tree_traits::value_traits;
  using pointer = typename tree_traits::const_pointer;
  using reference = typename tree_traits::const_reference;
  using base_ptr = typename tree_traits::base_ptr;
  using node_ptr = typename tree_traits::node_ptr;

  using iterator = RbTreeIterator<T>;
  using const_iterator = RbTreeConstIterator<T>;
  using self = const_iterator;

  using RbTreeIteratorBase<T>::node;

  // 构造函数
  RbTreeConstIterator() {}
  RbTreeConstIterator(base_ptr x) { node = x; }
  RbTreeConstIterator(node_ptr x) { node = x; }
  RbTreeConstIterator(const iterator& rhs) { node = rhs.node; }
  RbTreeConstIterator(const const_iterator& rhs) { node = rhs.node; }

  // 重载操作符
  reference operator*() const { return node->get_node_ptr()->value; }
  pointer operator->() const { return &(operator*()); }

  self& operator++() {
    this->inc();
    return *this;
  }

  self operator++(int) {
    self tmp(*this);
    this->inc();
    return tmp;
  }

  self& operator--() {
    this->dec();
    return *this;
  }

  self operator--(int) {
    self tmp(*this);
    this->dec();
    return tmp;
  }
};

// tree algorithm
template <typename NodePtr>
NodePtr rb_tree_min(NodePtr x) noexcept {
  while (x->left != nullptr) {
    x = x->left;
  }
  return x;
}

template <typename NodePtr>
NodePtr rb_tree_max(NodePtr x) noexcept {
  while (x->right != nullptr) {
    x = x->right;
  }
  return x;
}

template <typename NodePtr>
bool rb_tree_is_lchild(NodePtr node) noexcept {
  return node == node->parent->left;
}

template <typename NodePtr>
bool rb_tree_is_red(NodePtr node) noexcept {
  return node->color == kRbTreeRed;
}

template <typename NodePtr>
void rb_tree_set_black(NodePtr node) noexcept {
  node->color = kRbTreeBlack;
}

template <typename NodePtr>
void rb_tree_set_red(NodePtr node) noexcept {
  node->color = kRbTreeRed;
}

template <typename NodePtr>
NodePtr rb_tree_next(NodePtr node) noexcept {
  // TODO(lzj): 和inc()有何区别
  if (node->right != nullptr) {
    return rb_tree_min(node->right);
  }
  while (!rb_tree_is_lchild(node)) {
    node = node->parent;
  }
  return node->parent;
}

}  // namespace mystl

#endif  // ! MYTINYSTL_RB_TREE_H_
