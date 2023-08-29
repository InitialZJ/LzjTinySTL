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
#include <type_traits>

#include "allocator.h"
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

  reference operator*() const { return node_->as_node()->value; }
  pointer operator->() const { return &(operator*()); }

  // 前缀调用
  self& operator++() {
    MYSTL_DEBUG(node_ != nullptr);
    node_ = node_->next;
    return *this;
  }

  // 后缀调用
  self operator++(int) {
    self tmp = *this;
    ++*this;
    return tmp;
  }

  self& operator--() {
    MYSTL_DEBUG(node_ != nullptr);
    node_ = node_->prev;
    return *this;
  }

  self operator--(int) {
    self tmp = *this;
    --*this;
    return tmp;
  }

  bool operator==(const self& rhs) const { return node_ == rhs.node_; }
  bool operator!=(const self& rhs) const { return node_ != rhs.node_; }
};

template <typename T>
struct ListConstIterator : public Iterator<BidirectionalIteratorTag, T> {
  using value_type = T;
  using pointer = const T*;
  using reference = const T&;
  using base_ptr = typename NodeTraits<T>::base_ptr;
  using node_ptr = typename NodeTraits<T>::node_ptr;
  using self = ListConstIterator<T>;

  base_ptr node_;

  ListConstIterator() = default;
  explicit ListConstIterator(base_ptr x) : node_(x) {}
  explicit ListConstIterator(node_ptr x) : node_(x->as_base()) {}
  explicit ListConstIterator(const ListIterator<T>& rhs) : node_(rhs.node_) {}
  ListConstIterator(const ListConstIterator& rhs) : node_(rhs.node_) {}

  reference operator*() const { return node_->as_node()->value; }
  pointer operator->() const { return &(operator*()); }

  self& operator++() {
    MYSTL_DEBUG(node_ != nullptr);
    node_ = node_->next;
    return *this;
  }

  self operator++(int) {
    self tmp = *this;
    ++*this;
    return tmp;
  }

  self& operator--() {
    MYSTL_DEBUG(node_ != nullptr);
    node_ = node_->prev;
    return *this;
  }

  self operator--(int) {
    self tmp = *this;
    --*this;
    return tmp;
  }

  bool operator==(const self& rhs) const { return node_ == rhs.node_; }
  bool operator!=(const self& rhs) const { return node_ != rhs.node_; }
};

template <typename T>
class List {
 public:
  using allocator_type = mystl::Allocator<T>;
  using data_allocator = mystl::Allocator<T>;
  using base_allocator = mystl::Allocator<ListNodeBase<T>>;
  using node_allocator = mystl::Allocator<ListNode<T>>;

  using value_type = typename allocator_type::value_type;
  using pointer = typename allocator_type::poninter;
  using const_pointer = typename allocator_type::const_pointer;
  using reference = typename allocator_type::reference;
  using const_reference = typename allocator_type::const_reference;
  using size_type = typename allocator_type::size_type;
  using difference_type = typename allocator_type::difference_type;

  using iterator = ListIterator<T>;
  using const_iterator = ListConstIterator<T>;
  using reverse_iterator = mystl::ReverseIterator<iterator>;
  using const_reverse_iterator = mystl::ReverseIterator<const_iterator>;

  using base_ptr = typename NodeTraits<T>::base_ptr;
  using node_ptr = typename NodeTraits<T>::node_ptr;

  allocator_type get_allocator() { return node_allocator(); }

 private:
  base_ptr node_;   // 指向末尾结点
  size_type size_;  // 大小

 public:
  // 构造、复制、移动、析构函数
  List() { fill_init(0, value_type()); }

  explicit List(size_type n) { fill_init(n, value_type()); }

  List(size_type n, const T& value) { fill_init(n, value); }

  template <
      typename Iter,
      typename std::enable_if<mystl::IsInputIterator<Iter>::kValue, int>::type = 0>
  List(Iter first, Iter last) {
    copy_init(first, last);
  }

  List(std::initializer_list<T> ilist) { copy_init(ilist.begin(), ilist.end()); }

  List(const List& rhs) { copy_init(rhs.cbegin(), rhs.cend()); }

  List(List&& rhs) noexcept : node_(rhs.node_), size_(rhs.size_) {
    rhs.node_ = nullptr;
    rhs.size_ = 0;
  }

  List& operator=(const List& rhs) {
    if (this != &rhs) {
      assign(rhs.begin(), rhs.end());
    }
    return *this;
  }

  List& operator=(List&& rhs) noexcept {
    clear();
    splice(end(), rhs);
    return *this;
  }

  List& operator=(std::initializer_list<T> ilist) {
    List tmp = (ilist.begin(), ilist.end());
    swap(tmp);
    return *this;
  }

  ~List() {
    if (node_) {
      clear();
      base_allocator::deallocate(node_);
      node_ = nullptr;
      size_ = 0;
    }
  }

 public:
  // 迭代器相关操作
  iterator begin() noexcept { return node_->next; }

  const_iterator begin() const noexcept { return node_->next; }

  iterator end() noexcept { return node_; }

  const_iterator end() const noexcept { return node_; }

  reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }

  const_reverse_iterator rbegin() const noexcept { return reverse_iterator(end()); }

  reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

  const_reverse_iterator rend() const noexcept { return reverse_iterator(begin()); }

  const_iterator cbegin() const noexcept { return begin(); }

  const_iterator cend() const noexcept { return end(); }

  const_reverse_iterator crbegin() const noexcept { return rbegin(); }

  const_reverse_iterator crend() const noexcept { return rend(); }

  // 容量相关操作
  bool empty() const noexcept { return node_->next == node_; }

  size_type size() const noexcept { return size_; }

  size_type max_size() const noexcept { return static_cast<size_type>(-1); }

  // 访问元素相关操作
  reference front() {
    MYSTL_DEBUG(!empty());
    return *begin();
  }

  const_reference front() const {
    MYSTL_DEBUG(!empty());
    return *begin();
  }

  reference back() {
    MYSTL_DEBUG(!empty());
    return *(--end());
  }

  const_reference back() const {
    MYSTL_DEBUG(!empty());
    return *(--end());
  }

  // 调整容器相关操作
  // assign
  void assign(size_type n, const value_type& value) { fill_assign(n, value); }

  template <
      typename Iter,
      typename std::enable_if<mystl::IsInputIterator<Iter>::kValue, int>::type = 0>
  void assign(Iter first, Iter last) {
    copy_assign(first, last);
  }

  void assign(std::initializer_list<T> ilist) { copy_assign(ilist.begin(), ilist.end()); }

  // emplace_front / emplace_back / emplace
  template <typename... Args>
  void emplace_front(Args&&... args) {
    THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "List<T>'s size is too big");
    auto link_node = create_node(mystl::forward<Args>(args)...);
    link_nodes_at_front(link_node->as_base(), link_node->as_base());
    ++size_;
  }

  template <typename... Args>
  void emplace_back(Args&&... args) {
    THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "List<T>'s size is too big");
    auto link_node = create_node(mystl::forward<Args>(args)...);
    link_nodes_at_back(link_node->as_base(), link_node->as_base());
    ++size_;
  }

  template <typename... Args>
  iterator emplace(const_iterator pos, Args&&... args) {
    THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "List<T>'s size is too big");
    auto link_node = create_node(mystl::forward<Args>(args)...);
    link_nodes(pos.node_, link_node->as_base(), link_node->as_base());
    ++size_;
    return iterator(link_node);
  }

  // insert
  iterator insert(const_iterator pos, const value_type& value) {
    THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "List<T>'s size is too big");
    auto link_node = create_node(mystl::move(value));
    ++size_;
    return link_iter_node(pos, link_node->as_base());
  }

  iterator insert(const_iterator pos, value_type&& value) {
    THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "List<T>'s size is too big");
    auto link_node = create_node(mystl::move(value));
    ++size_;
    return link_iter_node(pos, link_node->as_base());
  }

  iterator insert(const_iterator pos, size_type n, const value_type& value) {
    THROW_LENGTH_ERROR_IF(size_ > max_size() - n, "List<T>'s size is too big");
    return fill_insert(pos, n, value);
  }

  template <
      typename Iter,
      typename std::enable_if<mystl::IsInputIterator<Iter>::kValue, int>::type = 0>
  iterator insert(const_iterator pos, Iter first, Iter last) {
    size_type n = mystl::distance(first, last);
    THROW_LENGTH_ERROR_IF(size_ > max_size() - n, "List<T>'s size is too big");
    return copy_insert(pos, n, first);
  }

  // push_front / push_back
  void push_front(const value_type& value) {
    THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "List<T>'s size is too big");
    auto link_node = create_node(value);
    link_nodes_at_front(link_node->as_base(), link_node->as_base);
    ++size_;
  }

  void push_front(value_type&& value) { emplace_front(mystl::move(value)); }

  void push_back(const value_type& value) {
    THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "List<T>'s size is too big");
    auto link_node = create_node(value);
    link_nodes_at_back(link_node->as_base(), link_node->as_base());
    ++size_;
  }

  void push_back(value_type&& value) { emplace_back(mystl::move(value)); }

  // pop_front / pop_back
};

}  // namespace mystl

#endif  // !MYTINYSTL_LIST_H_