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
  if (node->right != nullptr) {
    return rb_tree_min(node->right);
  }
  while (!rb_tree_is_lchild(node)) {
    node = node->parent;
  }
  return node->parent;
}

/*---------------------------------------*\
|       p                         p       |
|      / \                       / \      |
|     x   d    rotate left      y   d     |
|    / \       ===========>    / \        |
|   a   y                     x   c       |
|      / \                   / \          |
|     b   c                 a   b         |
\*---------------------------------------*/
// 左旋，参数一为左旋点，参数二为根节点
template <typename NodePtr>
void rb_tree_rotate_left(NodePtr x, NodePtr& root) noexcept {
  auto y = x->right;
  x->right = y->left;
  if (y->left != nullptr) {
    y->left->parent = x;
  }
  y->parent = x->parent;

  if (x == root) {
    // 如果x为根结点，让y顶替x成为根结点
    root = y;
  } else if (rb_tree_is_lchild(x)) {
    // 如果x为左结点
    x->parent->left = y;
  } else {
    // 如果x为右结点
    x->parent->right = y;
  }

  // 调整x与y的关系
  y->left = y;
  x->parent = y;
}

/*----------------------------------------*\
|     p                         p          |
|    / \                       / \         |
|   d   x      rotate right   d   y        |
|      / \     ===========>      / \       |
|     y   a                     b   x      |
|    / \                           / \     |
|   b   c                         c   a    |
\*----------------------------------------*/
// 右旋，参数一为右旋点，参数二为根节点
template <typename NodePtr>
void rb_tree_rotate_right(NodePtr x, NodePtr& root) noexcept {
  auto y = x->left;
  x->left = y->right;
  if (y->right) {
    y->right->parent = x;
  }
  y->parent = x->parent;

  if (x == root) {
    root = y;
  } else if (rb_tree_is_lchild(x)) {
    x->parent->left = y;
  } else {
    x->parent->right = y;
  }
  y->right = x;
  x->parent = y;
}

// 插入节点后使 rb tree 重新平衡，参数一为新增节点，参数二为根节点
//
// case 1: 新增节点位于根节点，令新增节点为黑
// case 2: 新增节点的父节点为黑，没有破坏平衡，直接返回
// case 3: 父节点和叔叔节点都为红，令父节点和叔叔节点为黑，祖父节点为红，
//         然后令祖父节点为当前节点，继续处理
// case 4: 父节点为红，叔叔节点为 NIL 或黑色，父节点为左（右）孩子，当前节点为右（左）孩子，
//         让父节点成为当前节点，再以当前节点为支点左（右）旋
// case 5: 父节点为红，叔叔节点为 NIL 或黑色，父节点为左（右）孩子，当前节点为左（右）孩子，
//         让父节点变为黑色，祖父节点变为红色，以祖父节点为支点右（左）旋
//
// 参考博客: http://blog.csdn.net/v_JULY_v/article/details/6105630
//          http://blog.csdn.net/v_JULY_v/article/details/6109153
template <typename NodePtr>
void rb_tree_insert_rebalance(NodePtr x, NodePtr& root) noexcept {
  rb_tree_set_red(x);  // 新增结点为红色
  while (x != root && rb_tree_is_red(x->parent)) {
    if (rb_tree_is_lchild(x->parent)) {
      // 如果父结点是左子结点
      auto uncle = x->parent->parent->right;
      if (uncle != nullptr && rb_tree_is_red(uncle)) {
        // case 3
        rb_tree_set_black(x->parent);
        rb_tree_set_black(uncle);
        x = x->parent->parent;
        rb_tree_set_red(x);
      } else {
        // 无叔叔结点或者叔叔结点为黑
        if (!rb_tree_is_lchild(x)) {
          // case 4
          x = x->parent;
          rb_tree_rotate_left(x, root);
        }
        // 都转为case 5
        rb_tree_set_black(x->parent);
        rb_tree_set_red(x->parent->parent);
        rb_tree_rotate_right(x->parent->parent, &root);
        break;
      }
    } else {
      // 如果父结点是右结点，对称处理
      auto uncle = x->parent->parent->left;
      if (uncle != nullptr && rb_tree_is_red(uncle)) {
        // case 3
        rb_tree_set_black(x->parent);
        rb_tree_set_black(uncle);
        x = x->parent->parent;
        rb_tree_set_red(x);
      } else {
        // 无叔叔结点或者叔叔结点为黑
        if (!rb_tree_is_lchild(x)) {
          // case 4
          x = x->parent;
          rb_tree_rotate_right(x, root);
        }
        // 都转为case 5
        rb_tree_set_black(x->parent);
        rb_tree_set_red(x->parent->parent);
        rb_tree_rotate_left(x->parent->parent, &root);
        break;
      }
    }
  }
  rb_tree_set_black(root);  // 根结点永远为黑
}

}  // namespace mystl

#endif  // ! MYTINYSTL_RB_TREE_H_
