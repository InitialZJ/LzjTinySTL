#ifndef MYTINYSTL_RB_TREE_H_
#define MYTINYSTL_RB_TREE_H_

// 模板类 rb_tree
// rb_tree : 红黑树
// 对应书5.2节

#include <cassert>
#include <initializer_list>
#include <iostream>

#include "algobase.h"
#include "allocator.h"
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
    } else if (node->left != nullptr) {
      node = rb_tree_max(node->left);
    } else {
      // 非header结点，也无左子结点
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
  // RbTreeConstIterator(iterator rhs) { node = rhs.node; }
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
  y->left = x;
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
        rb_tree_rotate_right(x->parent->parent, root);
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
        if (rb_tree_is_lchild(x)) {
          // case 4
          x = x->parent;
          rb_tree_rotate_right(x, root);
        }
        // 都转为case 5
        rb_tree_set_black(x->parent);
        rb_tree_set_red(x->parent->parent);
        rb_tree_rotate_left(x->parent->parent, root);
        break;
      }
    }
  }
  rb_tree_set_black(root);  // 根结点永远为黑
}

// 删除节点后使 rb tree
// 重新平衡，参数一为要删除的节点，参数二为根节点，参数三为最小节点，参数四为最大节点
//
// 参考博客: http://blog.csdn.net/v_JULY_v/article/details/6105630
//          http://blog.csdn.net/v_JULY_v/article/details/6109153
template <typename NodePtr>
NodePtr rb_tree_erase_rebalance(NodePtr z, NodePtr& root, NodePtr& leftmost, NodePtr& rightmost) {
  // y是可能的替换结点，指向最终要删除的结点
  auto y = (z->left == nullptr || z->right == nullptr) ? z : rb_tree_next(z);
  // x是y的一个独子结点或NIL结点
  auto x = y->left != nullptr ? y->left : y->right;
  // xp为x的父结点
  NodePtr xp = nullptr;

  // y != z说明z有两个非空子结点，此时y指向z右子树的最左结点，x指向y的右子结点
  // 用y顶替z的位置，用x顶替y的位置，最后用y指向z
  if (y != z) {
    z->left->parent = y;
    y->left = z->left;

    // 如果y不是z的右子结点，那么z的右子结点一定有左孩子
    if (y != z->right) {
      // x替换y的位置
      xp = y->parent;
      if (x != nullptr) {
        x->parent = y->parent;
      }

      y->parent->left = x;
      y->right = z->right;
      z->right->parent = y;
    } else {
      xp = y;
    }

    // 连接y与z的父结点
    if (root == z) {
      root = y;
    } else if (rb_tree_is_lchild(z)) {
      z->parent->left = y;
    } else {
      z->parent->right = y;
    }
    y->parent = z->parent;
    mystl::swap(y->color, z->color);
    y = z;
  } else {
    // y == z说明z至多只有一个孩子
    xp = y->parent;
    if (x) {
      x->parent = y->parent;
    }

    // 连接x与z的父结点
    if (root == z) {
      root = x;
    } else if (rb_tree_is_lchild(z)) {
      z->parent->left = x;
    } else {
      z->parent->right = x;
    }

    // 此时z有可能是最左结点或最右结点，更新数据
    if (leftmost == z) {
      leftmost = x == nullptr ? xp : rb_tree_min(x);
    }
    if (rightmost == z) {
      rightmost = x == nullptr ? xp : rb_tree_max(x);
    }
  }

  // 此时，y 指向要删除的节点，x 为替代节点，从 x 节点开始调整。
  // 如果删除的节点为红色，树的性质没有被破坏，否则按照以下情况调整（x 为左子节点为例）：
  // case 1: 兄弟节点为红色，令父节点为红，兄弟节点为黑，进行左（右）旋，继续处理
  // case 2: 兄弟节点为黑色，且两个子节点都为黑色或
  // NIL，令兄弟节点为红，父节点成为当前节点，继续处理 case 3: 兄弟节点为黑色，左子节点为红色或
  // NIL，右子节点为黑色或 NIL，
  //         令兄弟节点为红，兄弟节点的左子节点为黑，以兄弟节点为支点右（左）旋，继续处理
  // case 4:
  // 兄弟节点为黑色，右子节点为红色，令兄弟节点为父节点的颜色，父节点为黑色，兄弟节点的右子节点
  //         为黑色，以父节点为支点左（右）旋，树的性质调整完成，算法结束
  if (!rb_tree_is_red(y)) {
    // x为黑色时，调整，否则直接将x变成黑色即可
    while (x != root && (x == nullptr || !rb_tree_is_red(x))) {
      if (x == xp->left) {
        // 如果x为左子结点
        auto brother = xp->right;
        if (rb_tree_is_red(brother)) {
          // case 1
          rb_tree_set_black(brother);
          rb_tree_set_red(xp);
          rb_tree_rotate_left(xp, root);
          brother = xp->right;
        }
        // case 1转为了case 2、3、4中的一种
        if ((brother->left == nullptr || !rb_tree_is_red(brother->left)) &&
            (brother->right == nullptr || !rb_tree_is_red(brother->right))) {
          // case 2
          rb_tree_set_red(brother);
          x = xp;
          xp = xp->parent;
        } else {
          if (brother->right == nullptr || !rb_tree_is_red(brother->right)) {
            // case 3
            if (brother->left != nullptr) {
              rb_tree_set_black(brother->left);
            }
            rb_tree_set_red(brother);
            rb_tree_rotate_right(brother, root);
            brother = xp->right;
          }
          // 转为case 4
          brother->color = xp->color;
          rb_tree_set_black(xp);
          if (brother->right != nullptr) {
            rb_tree_set_black(brother->right);
          }
          rb_tree_rotate_left(xp, root);
          break;
        }
      } else {
        // x为右子结点，对称处理
        auto brother = xp->left;
        if (rb_tree_is_red(brother)) {
          // case 1
          rb_tree_set_black(brother);
          rb_tree_set_red(xp);
          rb_tree_rotate_right(xp, root);
          brother = xp->left;
        }
        if ((brother->left == nullptr || !rb_tree_is_red(brother->left)) &&
            (brother->right == nullptr || !rb_tree_is_red(brother->right))) {
          // case 2
          rb_tree_set_red(brother);
          x = xp;
          xp = xp->parent;
        } else {
          if (brother->left == nullptr || !rb_tree_is_red(brother->left)) {
            // case 3
            if (brother->right != nullptr) {
              rb_tree_set_black(brother->right);
            }
            rb_tree_set_red(brother);
            rb_tree_rotate_left(brother, root);
            brother = xp->left;
          }
          // 转为case 4
          brother->color = xp->color;
          rb_tree_set_black(xp);
          if (brother->left != nullptr) {
            rb_tree_set_black(brother->left);
          }
          rb_tree_rotate_right(xp, root);
          break;
        }
      }
    }
    if (x != nullptr) {
      rb_tree_set_black(x);
    }
  }
  return y;
}

// 模板类rb_tree
// 参数一代表数据类型，参数二代表键值比较类型
template <typename T, typename Compare>
class RbTree {
 public:
  // RbTree的嵌套型别定义
  using tree_traits = RbTreeTraits<T>;
  using value_traits = RbTreeValueTraits<T>;

  using base_type = typename tree_traits::base_type;
  using base_ptr = typename tree_traits::base_ptr;
  using node_type = typename tree_traits::node_type;
  using node_ptr = typename tree_traits::node_ptr;
  using key_type = typename tree_traits::key_type;
  using mapped_type = typename tree_traits::mapped_type;
  using value_type = typename tree_traits::value_type;
  using key_compare = Compare;

  using allocator_type = mystl::Allocator<T>;
  using data_allocator = mystl::Allocator<T>;
  using base_allocator = mystl::Allocator<base_type>;
  using node_allocator = mystl::Allocator<node_type>;

  using pointer = typename allocator_type::pointer;
  using const_pointer = typename allocator_type::const_pointer;
  using reference = typename allocator_type::reference;
  using const_reference = typename allocator_type::const_reference;
  using size_type = typename allocator_type::size_type;
  using difference_type = typename allocator_type::difference_type;

  using iterator = RbTreeIterator<T>;
  using const_iterator = RbTreeConstIterator<T>;
  using reverse_iterator = mystl::ReverseIterator<iterator>;
  using const_reverse_iterator = mystl::ReverseIterator<const_iterator>;

  allocator_type get_allocator() const { return node_allocator(); }
  key_compare key_comp() const { return key_comp_; }

 private:
  // 用以下三个数据表现rb tree
  base_ptr header_;       // 特殊结点，与根结点互为对方的父结点
  size_type node_count_;  // 结点数
  key_compare key_comp_;  // 结点键值比较的准则

 private:
  // 以下三个函数用于取得根结点，最小结点和最大结点
  base_ptr& root() const { return header_->parent; }
  base_ptr& leftmost() const { return header_->left; }
  base_ptr& rightmost() const { return header_->right; }

 public:
  // 构造、复制、析构函数
  RbTree() { rb_tree_init(); }

  RbTree(const RbTree& rhs);
  RbTree(RbTree&& rhs) noexcept;

  RbTree& operator=(const RbTree& rhs);
  RbTree& operator=(RbTree&& rhs);

  ~RbTree() { clear(); }

 public:
  // 迭代器相关操作
  iterator begin() noexcept { return leftmost(); }
  const_iterator begin() const noexcept { return leftmost(); }
  iterator end() noexcept { return header_; }
  const_iterator end() const noexcept { return header_; }

  reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
  reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

  const_iterator cbegin() const noexcept { return begin(); }
  const_iterator cend() const noexcept { return end(); }
  const_reverse_iterator crbegin() const noexcept { return rbegin(); }
  const_reverse_iterator crend() const noexcept { return rend(); }

  // 容量相关操作
  bool empty() const noexcept { return node_count_ == 0; }
  size_type size() const noexcept { return node_count_; }
  size_type max_size() const noexcept { return static_cast<size_type>(-1); }

  // 插入删除相关操作
  // emplace
  template <typename... Args>
  iterator emplace_multi(Args&&... args);

  template <typename... Args>
  mystl::pair<iterator, bool> emplace_unique(Args&&... args);

  template <typename... Args>
  iterator emplace_multi_use_hint(iterator hint, Args&&... args);

  template <typename... Args>
  iterator emplace_unique_use_hint(iterator hint, Args&&... args);

  // insert
  iterator insert_multi(const value_type& value);
  iterator insert_multi(value_type&& value) { return emplace_multi(mystl::move(value)); }

  iterator insert_multi(iterator hint, const value_type& value) {
    return emplace_multi_use_hint(hint, value);
  }
  iterator insert_multi(iterator hint, value_type&& value) {
    return emplace_multi_use_hint(hint, mystl::move(value));
  }

  template <typename InputIterator>
  void insert_multi(InputIterator first, InputIterator last) {
    size_type n = mystl::distance(first, last);
    THROW_LENGTH_ERROR_IF(node_count_ > max_size() - n, "RbTree<T, Comp>'s size too big");
    for (; n > 0; --n, ++first) {
      insert_multi(end(), *first);
    }
  }

  mystl::pair<iterator, bool> insert_unique(const value_type& value);
  mystl::pair<iterator, bool> insert_unique(value_type&& value) {
    return emplace_unique(mystl::move(value));
  }

  iterator insert_unique(iterator hint, const value_type& value) {
    return emplace_unique_use_hint(hint, value);
  }
  iterator insert_unique(iterator hint, value_type&& value) {
    return emplace_unique_use_hint(hint, mystl::move(value));
  }

  template <typename InputIterator>
  void insert_unique(InputIterator first, InputIterator last) {
    size_type n = mystl::distance(first, last);
    THROW_LENGTH_ERROR_IF(node_count_ > max_size() - n, "RbTree<T, Comp>'s size too big");
    for (; n > 0; --n, ++first) {
      insert_unique(end(), *first);
    }
  }

  // erase
  iterator erase(iterator hint);

  size_type erase_multi(const key_type& key);
  size_type erase_unique(const key_type& key);

  void erase(iterator first, iterator last);

  void clear();

  // rb_tree相关操作
  iterator find(const key_type& key);
  const_iterator find(const key_type& key) const;

  size_type count_multi(const key_type& key) const {
    auto p = equal_range_multi(key);
    return static_cast<size_type>(mystl::distance(p.first, p.second));
  }
  size_type count_unique(const key_type& key) const { return find(key) != end() ? 1 : 0; }

  iterator lower_bound(const key_type& key);
  const_iterator lower_bound(const key_type& key) const;

  iterator upper_bound(const key_type& key);
  const_iterator upper_bound(const key_type& key) const;

  mystl::pair<iterator, iterator> equal_range_multi(const key_type& key) {
    return mystl::pair<iterator, iterator>(lower_bound(key), upper_bound(key));
  }
  mystl::pair<const_iterator, const_iterator> equal_range_multi(const key_type& key) const {
    return mystl::pair<const_iterator, const_iterator>(lower_bound(key), upper_bound(key));
  }

  mystl::pair<iterator, iterator> equal_range_unique(const key_type& key) {
    iterator it = find(key);
    auto next = it;
    return it == end() ? mystl::make_pair(it, it) : mystl::make_pair(it, ++next);
  }
  mystl::pair<const_iterator, const_iterator> equal_range_unique(const key_type& key) const {
    iterator it = find(key);
    auto next = it;
    return it == end() ? mystl::make_pair(it, it) : mystl::make_pair(it, ++next);
  }

  void swap(RbTree& rhs) noexcept;

 private:
  // node related
  template <typename... Args>
  node_ptr create_node(Args&&... args);
  node_ptr clone_node(base_ptr);
  void destroy_node(node_ptr p);

  // init / reset
  void rb_tree_init();
  void reset();

  // get insert pos
  mystl::pair<base_ptr, bool> get_insert_multi_pos(const key_type& key);
  mystl::pair<mystl::pair<base_ptr, bool>, bool> get_insert_unique_pos(const key_type& key);

  // insert value / insert node
  iterator insert_value_at(base_ptr x, const value_type& value, bool add_to_left);
  iterator insert_node_at(base_ptr x, node_ptr node, bool add_to_left);

  // insert use hint
  iterator insert_multi_use_hint(iterator hint, key_type key, node_ptr node);
  iterator insert_unique_use_hint(iterator hint, key_type key, node_ptr node);

  // copy tree / erase tree
  base_ptr copy_from(base_ptr x, base_ptr p);
  void erase_since(base_ptr x);
};

// 复制构造函数
template <typename T, typename Compare>
RbTree<T, Compare>::RbTree(const RbTree& rhs) {
  rb_tree_init();
  if (rhs.node_count_ != 0) {
    root() = copy_from(rhs.root(), header_);
    leftmost() = rb_tree_min(root());
    rightmost() = rb_tree_max(root());
  }
  node_count_ = rhs.node_count_;
  key_comp_ = rhs.key_comp_;
}

// 移动构造函数
template <typename T, typename Compare>
RbTree<T, Compare>::RbTree(RbTree&& rhs) noexcept
    : header_(mystl::move(rhs.header_)), node_count_(rhs.node_count_), key_comp_(rhs.key_comp_) {
  rhs.reset();
}

// 复制赋值操作符
template <typename T, typename Compare>
RbTree<T, Compare>& RbTree<T, Compare>::operator=(const RbTree& rhs) {
  if (this != &rhs) {
    clear();

    if (rhs.node_count_ != 0) {
      root() = copy_from(rhs.root(), header_);
      leftmost() = rb_tree_min(root());
      rightmost() = rb_tree_max(root());
    }

    node_count_ = rhs.node_count_;
    key_comp_ = rhs.key_comp_;
  }
  return *this;
}

// 移动赋值操作符
template <typename T, typename Compare>
RbTree<T, Compare>& RbTree<T, Compare>::operator=(RbTree&& rhs) {
  clear();
  header_ = mystl::move(rhs.header_);
  node_count_ = rhs.node_count_;
  key_comp_ = rhs.key_comp_;
  rhs.reset();
  return *this;
}

// 就地插入元素，键值允许重复
template <typename T, typename Compare>
template <typename... Args>
typename RbTree<T, Compare>::iterator RbTree<T, Compare>::emplace_multi(Args&&... args) {
  THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "RbTree<T, Comp>'s size too big");
  node_ptr np = create_node(mystl::forward<Args>(args)...);
  auto res = get_insert_multi_pos(value_traits::get_key(np->value));
  return insert_node_at(res.first, np, res.second);
}

// 就地插入元素，键值不允许重复
template <typename T, typename Compare>
template <typename... Args>
mystl::pair<typename RbTree<T, Compare>::iterator, bool> RbTree<T, Compare>::emplace_unique(
    Args&&... args) {
  THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "RbTree<T, Comp>'s size too big");
  node_ptr np = create_node(mystl::forward<Args>(args)...);
  auto res = get_insert_unique_pos(value_traits::get_key(np->value));
  if (res.second) {
    // 插入成功
    return mystl::make_pair(insert_node_at(res.first.first, np, res.first.second), true);
  }
  destroy_node(np);
  return mystl::make_pair(iterator(res.first.first), false);
}

// 就地插入元素，键值允许重复，当hint位置与插入位置接近时，，插入操作的时间复杂度可以降低
template <typename T, typename Compare>
template <typename... Args>
typename RbTree<T, Compare>::iterator RbTree<T, Compare>::emplace_multi_use_hint(
    iterator hint, Args&&... args) {
  THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "RbTree<T, Comp>'s size too big");
  node_ptr np = create_node(mystl::forward<Args>(args)...);
  if (node_count_ == 0) {
    return insert_node_at(header_, np, true);
  }
  key_type key = value_traits::get_key(np->value);
  if (hint == begin()) {
    // 位于begin处
    if (key_comp_(key, value_traits::get_key(*hint))) {
      return insert_node_at(hint.node, np, true);
    } else {
      auto pos = get_insert_multi_pos(key);
      return insert_node_at(pos.first, np, pos.second);
    }
  } else if (hint == end()) {
    // 位于end处
    if (!key_comp_(key, value_traits::get_key(rightmost()->get_node_ptr()->value))) {
      return insert_node_at(rightmost(), np, false);
    } else {
      auto pos = get_insert_multi_pos(key);
      return insert_node_at(pos.first, np, pos.second);
    }
  }
  return insert_multi_use_hint(hint, key, np);
}

// 就地插入元素，键值不允许重复，当hint位置与插入位置接近时，插入操作的时间复杂度可以降低
template <typename T, typename Compare>
template <typename... Args>
typename RbTree<T, Compare>::iterator RbTree<T, Compare>::emplace_unique_use_hint(
    iterator hint, Args&&... args) {
  THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "RbTree<T, Comp>'s size too big");
  node_ptr np = create_node(mystl::forward<Args>(args)...);
  if (node_count_ == 0) {
    return insert_node_at(header_, np, true);
  }
  key_type key = value_traits::get_key(np->value);
  if (hint == begin()) {
    // 位于begin处
    if (key_comp_(key, value_traits::get_key(*hint))) {
      return insert_node_at(hint.node, np, true);
    } else {
      auto pos = get_insert_unique_pos(key);
      if (!pos.second) {
        destroy_node(np);
        return pos.first.first;
      }
      return insert_node_at(pos.first.first, np, pos.first.second);
    }
  } else if (hint == end()) {
    // 位于end处
    if (key_comp_(value_traits::get_key(rightmost()->get_node_ptr()->value), key)) {
      return insert_node_at(rightmost(), np, false);
    } else {
      auto pos = get_insert_unique_pos(key);
      if (!pos.second) {
        destroy_node(np);
        return pos.first.first;
      }
      return insert_node_at(pos.first.first, np, pos.first.second);
    }
  }
  return insert_unique_use_hint(hint, key, np);
}

// 插入元素，结点键值允许重复
template <typename T, typename Compare>
typename RbTree<T, Compare>::iterator RbTree<T, Compare>::insert_multi(const value_type& value) {
  THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "RbTree<T, Comp>'s size too big");
  auto res = get_insert_multi_pos(value_traits::get_key(value));
  return insert_value_at(res.first, value, res.second);
}

// 插入新值，结点键值不允许重复，返回一个pair，若插入成功，pair的第二个参数为true，否则为false
template <typename T, typename Compare>
mystl::pair<typename RbTree<T, Compare>::iterator, bool> RbTree<T, Compare>::insert_unique(
    const value_type& value) {
  THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "RbTree<T, Comp>'s size too big");
  auto res = get_insert_unique_pos(value_traits::get_key(value));
  if (res.second) {
    // 插入成功
    return mystl::make_pair(insert_value_at(res.first.first, value, res.first.second), true);
  }
  return mystl::make_pair(res.first.first, false);
}

// 删除hint位置的结点
template <typename T, typename Compare>
typename RbTree<T, Compare>::iterator RbTree<T, Compare>::erase(iterator hint) {
  auto node = hint.node->get_node_ptr();
  iterator next(node);
  ++next;
  rb_tree_erase_rebalance(hint.node, root(), leftmost(), rightmost());
  destroy_node(node);
  --node_count_;
  return next;
}

// 删除键值等于key的元素，返回删除的个数
template <typename T, typename Compare>
typename RbTree<T, Compare>::size_type RbTree<T, Compare>::erase_multi(const key_type& key) {
  auto p = equal_range_multi(key);
  size_type n = mystl::distance(p.first, p.second);
  erase(p.first, p.second);
  return n;
}

// 删除键值等于key的元素，返回删除的个数
template <typename T, typename Compare>
typename RbTree<T, Compare>::size_type RbTree<T, Compare>::erase_unique(const key_type& key) {
  auto it = find(key);
  if (it != end()) {
    erase(it);
    return 1;
  }
  return 0;
}

// 删除[first, last)区间内的元素
template <typename T, typename Compare>
void RbTree<T, Compare>::erase(iterator first, iterator last) {
  if (first == begin() && last == end()) {
    clear();
  } else {
    while (first != last) {
      erase(first++);
    }
  }
}

// 清空RbTree
template <typename T, typename Compare>
void RbTree<T, Compare>::clear() {
  if (node_count_ != 0) {
    erase_since(root());
    leftmost() = header_;
    root() = nullptr;
    rightmost() = header_;
    node_count_ = 0;
  }
}

// 查找键值为key的结点，返回指向它的迭代器
template <typename T, typename Compare>
typename RbTree<T, Compare>::iterator RbTree<T, Compare>::find(const key_type& key) {
  auto y = header_;  // 最后一个不小于key的结点
  auto x = root();
  while (x != nullptr) {
    if (!key_comp_(value_traits::get_key(x->get_node_ptr()->value), key)) {
      // key小于等于x键值，向左走
      y = x;
      x = x->left;
    } else {
      // key大于x键值，向右走
      x = x->right;
    }
  }
  iterator j = iterator(y);
  return (j == end() || key_comp_(key, value_traits::get_key(*j))) ? end() : j;
}

// 查找键值为key的结点，返回指向它的迭代器
template <typename T, typename Compare>
typename RbTree<T, Compare>::const_iterator RbTree<T, Compare>::find(const key_type& key) const {
  auto y = header_;  // 最后一个不小于key的结点
  auto x = root();
  while (x != nullptr) {
    if (!key_comp_(value_traits::get_key(x->get_node_ptr()->value), key)) {
      // key小于等于x键值，向左走
      y = x;
      x = x->left;
    } else {
      // key大于x键值，向右走
      x = x->right;
    }
  }
  const_iterator j = const_iterator(y);
  return (j == end() || key_comp_(key, value_traits::get_key(*j))) ? end() : j;
}

// 键值不小于key的第一个位置
template <typename T, typename Compare>
typename RbTree<T, Compare>::iterator RbTree<T, Compare>::lower_bound(const key_type& key) {
  auto y = header_;
  auto x = root();
  while (x != nullptr) {
    if (!key_comp_(value_traits::get_key(x->get_node_ptr()->value), key)) {
      // key <= x
      y = x;
      x = x->left;
    } else {
      x = x->right;
    }
  }
  return iterator(y);
}

// 键值不小于key的第一个位置
template <typename T, typename Compare>
typename RbTree<T, Compare>::const_iterator RbTree<T, Compare>::lower_bound(
    const key_type& key) const {
  auto y = header_;
  auto x = root();
  while (x != nullptr) {
    if (!key_comp_(value_traits::get_key(x->get_node_ptr()->value), key)) {
      // key <= x
      y = x;
      x = x->left;
    } else {
      x = x->right;
    }
  }
  return const_iterator(y);
}

// 键值不小于key的最后一个位置
template <typename T, typename Compare>
typename RbTree<T, Compare>::iterator RbTree<T, Compare>::upper_bound(const key_type& key) {
  auto y = header_;
  auto x = root();
  while (x != nullptr) {
    if (key_comp_(key, value_traits::get_key(x->get_node_ptr()->value))) {
      // key < x
      y = x;
      x = x->left;
    } else {
      x = x->right;
    }
  }
  return iterator(y);
}

// 键值不小于key的最后一个位置
template <typename T, typename Compare>
typename RbTree<T, Compare>::const_iterator RbTree<T, Compare>::upper_bound(
    const key_type& key) const {
  auto y = header_;
  auto x = root();
  while (x != nullptr) {
    if (key_comp_(key, value_traits::get_key(x->get_node_ptr()->value))) {
      // key < x
      y = x;
      x = x->left;
    } else {
      x = x->right;
    }
  }
  return const_iterator(y);
}

// 交换RbTree
template <typename T, typename Compare>
void RbTree<T, Compare>::swap(RbTree& rhs) noexcept {
  if (this != &rhs) {
    mystl::swap(header_, rhs.header_);
    mystl::swap(node_count_, rhs.node_count_);
    mystl::swap(key_comp_, rhs.key_comp_);
  }
}

// helper function

// 创建一个结点
template <typename T, typename Compare>
template <typename... Args>
typename RbTree<T, Compare>::node_ptr RbTree<T, Compare>::create_node(Args&&... args) {
  auto tmp = node_allocator::allocate(1);
  try {
    data_allocator::construct(mystl::address_of(tmp->value), mystl::forward<Args>(args)...);
    tmp->left = nullptr;
    tmp->right = nullptr;
    tmp->parent = nullptr;
  } catch (...) {
    node_allocator::deallocate(tmp);
    throw;
  }
  return tmp;
}

// 复制一个结点
template <typename T, typename Compare>
typename RbTree<T, Compare>::node_ptr RbTree<T, Compare>::clone_node(base_ptr x) {
  node_ptr tmp = create_node(x->get_node_ptr()->value);
  tmp->color = x->color;
  tmp->left = nullptr;
  tmp->right = nullptr;
  return tmp;
}

// 销毁一个结点
template <typename T, typename Compare>
void RbTree<T, Compare>::destroy_node(node_ptr p) {
  data_allocator::destroy(&p->value);
  node_allocator::deallocate(p);
}

// 初始化容器
template <typename T, typename Compare>
void RbTree<T, Compare>::rb_tree_init() {
  header_ = base_allocator::allocate(1);
  header_->color = kRbTreeRed;  // header_结点颜色为红，与root区分
  root() = nullptr;
  leftmost() = header_;
  rightmost() = header_;
  node_count_ = 0;
}

// reset函数
template <typename T, typename Compare>
void RbTree<T, Compare>::reset() {
  header_ = nullptr;
  node_count_ = 0;
}

// get_insert_multi_pos函数
template <typename T, typename Compare>
mystl::pair<typename RbTree<T, Compare>::base_ptr, bool> RbTree<T, Compare>::get_insert_multi_pos(
    const key_type& key) {
  auto x = root();
  auto y = header_;
  bool add_to_left = true;
  while (x != nullptr) {
    y = x;
    add_to_left = key_comp_(key, value_traits::get_key(x->get_node_ptr()->value));
    x = add_to_left ? x->left : x->right;
  }
  return mystl::make_pair(y, add_to_left);
}

// get_insert_unique_pos函数
template <typename T, typename Compare>
mystl::pair<mystl::pair<typename RbTree<T, Compare>::base_ptr, bool>, bool>
RbTree<T, Compare>::get_insert_unique_pos(const key_type& key) {
  // 返回一个pair，第一个值为一个pair，包含插入点的父结点和一个bool表示是否在左边插入，
  // 第二个值为一个bool，表示是否插入成功
  auto x = root();
  auto y = header_;
  bool add_to_left = true;  // 树为空时也在header_左边插入
  while (x != nullptr) {
    y = x;
    add_to_left = key_comp_(key, value_traits::get_key(x->get_node_ptr()->value));
    x = add_to_left ? x->left : x->right;
  }
  iterator j = iterator(y);
  if (add_to_left) {
    if (y == header_ || j == begin()) {
      // 如果树为空树或插入点在最左结点处，肯定可以插入新的结点
      return mystl::make_pair(mystl::make_pair(y, true), true);
    } else {
      // 否则，如果存在重复结点，那么--j就是重复的值
      --j;
    }
  }
  if (key_comp_(value_traits::get_key(*j), key)) {
    // 表明新结点没有重复
    return mystl::make_pair(mystl::make_pair(y, add_to_left), true);
  }
  // 进行至此，表示新结点与现有结点键值重复
  return mystl::make_pair(mystl::make_pair(y, add_to_left), false);
}

// insert_value_at函数
// x为插入点的父结点，value为要插入的值，add_to_left表示是否在左边插入
template <typename T, typename Compare>
typename RbTree<T, Compare>::iterator RbTree<T, Compare>::insert_value_at(
    base_ptr x, const value_type& value, bool add_to_left) {
  node_ptr node = create_node(value);
  node->parent = x;
  auto base_node = node->get_base_ptr();
  if (x == header_) {
    root() = base_node;
    leftmost() = base_node;
    rightmost() = base_node;
  } else if (add_to_left) {
    x->left = base_node;
    if (leftmost() == x) {
      leftmost() = base_node;
    }
  } else {
    x->right = base_node;
    if (rightmost() == x) {
      rightmost() = base_node;
    }
  }
  rb_tree_insert_rebalance(base_node, root());
  ++node_count_;
  return iterator(node);
}

// 在x结点处插入新的结点
// x为插入点的父结点，node为要插入的结点，add_to_left表示是否在左边插入
template <typename T, typename Compare>
typename RbTree<T, Compare>::iterator RbTree<T, Compare>::insert_node_at(
    base_ptr x, node_ptr node, bool add_to_left) {
  node->parent = x;
  auto base_node = node->get_base_ptr();
  if (x == header_) {
    root() = base_node;
    leftmost() = base_node;
    rightmost() = base_node;
  } else if (add_to_left) {
    x->left = base_node;
    if (leftmost() == x) {
      leftmost() = base_node;
    }
  } else {
    x->right = base_node;
    if (rightmost() == x) {
      rightmost() = base_node;
    }
  }
  rb_tree_insert_rebalance(base_node, root());
  ++node_count_;
  return iterator(node);
}

// 插入元素，键值允许重复，使用hint
template <typename T, typename Compare>
typename RbTree<T, Compare>::iterator RbTree<T, Compare>::insert_multi_use_hint(
    iterator hint, key_type key, node_ptr node) {
  // 在hint附近寻找可插入的位置
  auto np = hint.node;
  auto before = hint;
  --before;
  auto bnp = before.node;
  if (!key_comp_(key, value_traits::get_key(*before)) &&
      !key_comp_(value_traits::get_key(*hint), key)) {
    // before <= node <= hint
    if (bnp->right == nullptr) {
      return insert_node_at(bnp, node, false);
    } else if (np->left == nullptr) {
      return insert_node_at(np, node, true);
    }
  }
  auto pos = get_insert_multi_pos(key);
  return insert_node_at(pos.first, node, pos.second);
}

// 插入元素，键值不允许重复，使用hint
template <typename T, typename Compare>
typename RbTree<T, Compare>::iterator RbTree<T, Compare>::insert_unique_use_hint(
    iterator hint, key_type key, node_ptr node) {
  // 在hint附近寻找可插入的位置
  auto np = hint.node;
  auto before = hint;
  --before;
  auto bnp = before.node;
  if (key_comp_(value_traits::get_key(*before), key) &&
      key_comp_(key, value_traits::get_key(*hint))) {
    // before < node < hint
    if (bnp->right == nullptr) {
      return insert_node_at(bnp, node, false);
    } else if (np->left == nullptr) {
      return insert_node_at(np, node, true);
    }
  }
  auto pos = get_insert_unique_pos(key);
  if (!pos.second) {
    destroy_node(node);
    return pos.first.first;
  }
  return insert_node_at(pos.first.first, node, pos.first.second);
}

// copy_from函数
// 递归复制一棵树，结点从x开始，p为x的父结点
template <typename T, typename Compare>
typename RbTree<T, Compare>::base_ptr RbTree<T, Compare>::copy_from(base_ptr x, base_ptr p) {
  auto top = clone_node(x);
  top->parent = p;
  try {
    if (x->right) {
      top->right = copy_from(x->right, top);
    }
    p = top;
    x = x->left;
    while (x != nullptr) {
      auto y = clone_node(x);
      p->left = y;
      y->parent = p;
      if (x->right) {
        y->right = copy_from(x->right, y);
      }
      p = y;
      x = x->left;
    }
  } catch (...) {
    erase_since(top);
    throw;
  }
  return top;
}

// erase_since函数
// 从x结点开始删除该结点及其子树
template <typename T, typename Compare>
void RbTree<T, Compare>::erase_since(base_ptr x) {
  while (x != nullptr) {
    erase_since(x->right);
    auto y = x->left;
    destroy_node(x->get_node_ptr());
    x = y;
  }
}

// 重载比较操作符
template <typename T, typename Compare>
bool operator==(const RbTree<T, Compare>& lhs, const RbTree<T, Compare>& rhs) {
  return lhs.size() == rhs.size() && mystl::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename T, typename Compare>
bool operator<(const RbTree<T, Compare>& lhs, const RbTree<T, Compare>& rhs) {
  return mystl::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename T, typename Compare>
bool operator!=(const RbTree<T, Compare>& lhs, const RbTree<T, Compare>& rhs) {
  return !(lhs == rhs);
}

template <typename T, typename Compare>
bool operator>(const RbTree<T, Compare>& lhs, const RbTree<T, Compare>& rhs) {
  return rhs < lhs;
}

template <typename T, typename Compare>
bool operator<=(const RbTree<T, Compare>& lhs, const RbTree<T, Compare>& rhs) {
  return !(rhs < lhs);
}

template <typename T, typename Compare>
bool operator>=(const RbTree<T, Compare>& lhs, const RbTree<T, Compare>& rhs) {
  return !(lhs < rhs);
}

// 重载mystl的swap
template <typename T, typename Compare>
void swap(RbTree<T, Compare>& lhs, RbTree<T, Compare>& rhs) noexcept {
  lhs.swap(rhs);
}

}  // namespace mystl

#endif  // ! MYTINYSTL_RB_TREE_H_
