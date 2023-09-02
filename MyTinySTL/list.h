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

#include "algobase.h"
#include "allocator.h"
#include "exceptdef.h"
#include "functional.h"
#include "iterator.h"
#include "memory.h"
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

// list的结点结构
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
  ListNode(const T& v) : value(v) {}
  ListNode(T&& v) : value(mystl::move(v)) {}

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
  ListIterator(base_ptr x) : node_(x) {}
  ListIterator(node_ptr x) : node_(x->as_base()) {}
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
  ListConstIterator(base_ptr x) : node_(x) {}
  ListConstIterator(node_ptr x) : node_(x->as_base()) {}
  ListConstIterator(const ListIterator<T>& rhs) : node_(rhs.node_) {}
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
  using pointer = typename allocator_type::pointer;
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
    List tmp(ilist.begin(), ilist.end());
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
  [[nodiscard]] bool empty() const noexcept { return node_->next == node_; }

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
  void pop_front() {
    MYSTL_DEBUG(!empty());
    auto n = node_->next;
    unlink_nodes(n, n);
    destroy_node(n->as_node());
    --size_;
  }

  void pop_back() {
    MYSTL_DEBUG(!empty());
    auto n = node_->prev;
    unlink_nodes(n, n);
    destroy_node(n->as_node());
    --size_;
  }

  // erase / clear
  iterator erase(const_iterator pos);
  iterator erase(const_iterator first, const_iterator last);

  void clear();

  // resize
  void resize(size_type new_size) { resize(new_size, value_type()); }

  void resize(size_type new_size, const value_type& value);

  void swap(List& rhs) noexcept {
    mystl::swap(node_, rhs.node_);
    mystl::swap(size_, rhs.size_);
  }

  // list相关操作
  void splice(const_iterator pos, List& x);
  void splice(const_iterator pos, List& x, const_iterator it);
  void splice(const_iterator pos, List& x, const_iterator first, const_iterator last);

  void remove(const value_type& value) {
    remove_if([&](const value_type& v) { return v == value; });
  }

  template <typename UnaryPredicate>
  void remove_if(UnaryPredicate pred);

  void unique() { unique(mystl::EqualTo<T>()); }
  template <typename BinaryPredicate>
  void unique(BinaryPredicate pred);

  void merge(List& x) { merge(x, mystl::Less<T>()); }
  template <typename Compare>
  void merge(List& x, Compare comp);

  void sort() { list_sort(begin(), end(), size(), mystl::Less<T>()); }
  template <typename Compare>
  void sort(Compare comp) {
    list_sort(begin(), end(), size(), comp);
  }

  void reverse();

 private:
  // helper functions

  // create / destroy node
  template <typename... Args>
  node_ptr create_node(Args&&... args);
  void destroy_node(node_ptr p);

  // initialize
  void fill_init(size_type n, const value_type& value);
  template <typename Iter>
  void copy_init(Iter first, Iter last);

  // link / unlink
  iterator link_iter_node(const_iterator pos, base_ptr node);
  void link_nodes(base_ptr p, base_ptr first, base_ptr last);
  void link_nodes_at_front(base_ptr first, base_ptr last);
  void link_nodes_at_back(base_ptr first, base_ptr last);
  void unlink_nodes(base_ptr f, base_ptr l);

  // assign
  void fill_assign(size_type n, const value_type& value);
  template <typename Iter>
  void copy_assign(Iter f2, Iter l2);

  // insert
  iterator fill_insert(const_iterator pos, size_type n, const value_type& value);
  template <typename Iter>
  iterator copy_insert(const_iterator pos, size_type n, Iter first);

  // sort
  template <typename Compare>
  iterator list_sort(iterator f1, iterator l2, size_type n, Compare comp);
};

// 删除pos处的元素
template <typename T>
typename List<T>::iterator List<T>::erase(const_iterator pos) {
  MYSTL_DEBUG(pos != cend());
  auto n = pos.node_;
  auto next = n->next;
  unlink_nodes(n, n);
  destroy_node(n->as_node());
  --size_;
  return iterator(next);
}

// 删除[first, last)内的元素
template <typename T>
typename List<T>::iterator List<T>::erase(const_iterator first, const_iterator last) {
  if (first != last) {
    unlink_nodes(first.node_, last.node_->prev);
    while (first != last) {
      auto cur = first.node_;
      ++first;
      destroy_node(cur->as_node());
      --size_;
    }
  }
  return iterator(last.node_);
}

// 清空List
template <typename T>
void List<T>::clear() {
  if (size_ != 0) {
    auto cur = node_->next;
    for (base_ptr next = cur->next; cur != node_; cur = next, next = cur->next) {
      destroy(cur->as_node());
    }
    node_->unlink();
    size_ = 0;
  }
}

// 重置容器大小
template <typename T>
void List<T>::resize(size_type new_size, const value_type& value) {
  auto i = begin();
  size_type len = 0;
  while (i != end() && len < new_size) {
    ++i;
    ++len;
  }
  if (len == new_size) {
    erase(i, node_);
  } else {
    insert(node_, new_size - len, value);
  }
}

// 将List x接合于pos之前
template <typename T>
void List<T>::splice(const_iterator pos, List& x) {
  MYSTL_DEBUG(this != &x);
  if (!x.empty()) {
    THROW_LENGTH_ERROR_IF(size_ > max_size() - x.size_, "List<T>'s size is too big");
    auto f = x.node_->next;
    auto l = x.node_->prev;

    x.unlink_nodes(f, l);
    link_nodes(pos.node_, f, l);

    size_ += x.size_;
    x.size_ = 0;
  }
}

// 将it所指的结点接合于pos之前
template <typename T>
void List<T>::splice(const_iterator pos, List& x, const_iterator it) {
  if (pos.node_ != it.node_ && pos.node_ != it.node_->next) {
    THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "List<T>'s size is too big");

    auto f = it.node_;
    x.unlink_nodes(f, f);
    link_nodes(pos.node_, f, f);

    ++size_;
    --x.size_;
  }
}

// 将List x的[first, last)内的结点接合于pos之前
template <typename T>
void List<T>::splice(const_iterator pos, List& x, const_iterator first, const_iterator last) {
  if (first != last && this != &x) {
    size_type n = mystl::distance(first, last);
    THROW_LENGTH_ERROR_IF(size_ > max_size() - n, "List<T>'s size is too big");
    auto f = first.node_;
    auto l = last.node_->prev;

    x.unlink_nodes(f, l);
    link_nodes(pos.node_, f, l);

    size_ += n;
    x.size_ -= n;
  }
}

// 将另一元操作pred为true的所有元素移除
template <typename T>
template <typename UnaryPredicate>
void List<T>::remove_if(UnaryPredicate pred) {
  auto f = begin();
  auto l = end();
  for (auto next = f; f != l; f = next) {
    ++next;
    if (pred(*f)) {
      erase(f);
    }
  }
}

// 移除List中满足pred为true重复元素
template <typename T>
template <typename BinaryPredicate>
void List<T>::unique(BinaryPredicate pred) {
  auto i = begin();
  auto e = end();
  auto j = i;
  ++j;
  while (j != e) {
    if (pred(*i, *j)) {
      erase(j);
    } else {
      i = j;
    }
    j = i;
    ++j;
  }
}

// 与另一个List合并，按照comp为true的顺序
template <typename T>
template <typename Compare>
void List<T>::merge(List& x, Compare comp) {
  if (this != &x) {
    THROW_LENGTH_ERROR_IF(size_ > max_size() - x.size_, "list<T>'s size too big");

    auto f1 = begin();
    auto l1 = end();
    auto f2 = x.begin();
    auto l2 = x.end();

    while (f1 != l1 && f2 != l2) {
      if (comp(*f2, *f1)) {
        // 使comp为true的一段区间
        auto next = f2;
        ++next;
        for (; next != l2 && comp(*next, *f1); ++next) {
          ;
        }
        auto f = f2.node_;
        auto l = next.node_->prev;
        f2 = next;

        x.unlink_nodes(f, l);
        link_nodes(f1.node_, f, l);
        ++f1;
      } else {
        ++f1;
      }
    }

    // 连接剩余部分
    if (f2 != l2) {
      auto f = f2.node_;
      auto l = l2.node_->prev;
      x.unlink_nodes(f, l);
      link_nodes(l1.node_, f, l);
    }

    size_ += x.size_;
    x.size_ = 0;
  }
}

// 将List反转
template <typename T>
void List<T>::reverse() {
  if (size_ <= 1) {
    return;
  }
  auto i = begin();
  auto e = end();
  while (i.node_ != e.node_) {
    mystl::swap(i.node_->prev, i.node_->next);
    i.node_ = i.node_->prev;
  }
  mystl::swap(e.node_->prev, e.node_->next);
}

// helper functions

// 创建结点
template <typename T>
template <typename... Args>
typename List<T>::node_ptr List<T>::create_node(Args&&... args) {
  node_ptr p = node_allocator::allocate(1);
  try {
    data_allocator::construct(mystl::address_of(p->value), mystl::forward<Args>(args)...);
    p->prev = nullptr;
    p->next = nullptr;
  } catch (...) {
    node_allocator::deallocate(p);
    throw;
  }
  return p;
}

// 销毁结点
template <typename T>
void List<T>::destroy_node(node_ptr p) {
  data_allocator::destroy(mystl::address_of(p->value));
  node_allocator::deallocate(p);
}

// 用n个元素初始化容器
template <typename T>
void List<T>::fill_init(size_type n, const value_type& value) {
  node_ = base_allocator::allocate(1);
  node_->unlink();
  size_ = n;
  try {
    for (; n > 0; --n) {
      auto node = create_node(value);
      link_nodes_at_back(node->as_base(), node->as_base());
    }
  } catch (...) {
    clear();
    base_allocator::deallocate(node_);
    node_ = nullptr;
    throw;
  }
}

// 以[first, last)初始化容器
template <typename T>
template <typename Iter>
void List<T>::copy_init(Iter first, Iter last) {
  node_ = base_allocator::allocate(1);
  node_->unlink();
  size_type n = mystl::distance(first, last);
  size_ = n;
  try {
    for (; n > 0; --n, ++first) {
      auto node = create_node(*first);
      link_nodes_at_back(node->as_base(), node->as_base());
    }
  } catch (...) {
    clear();
    base_allocator::deallocate(node_);
    node_ = nullptr;
    throw;
  }
}

// 在pos处连接一个结点
template <typename T>
typename List<T>::iterator List<T>::link_iter_node(const_iterator pos, base_ptr link_node) {
  if (pos == node_->next) {
    link_nodes_at_front(link_node, link_node);
  } else if (pos == node_) {
    link_nodes_at_back(link_node, link_node);
  } else {
    link_nodes(pos.node_, link_node, link_node);
  }
  return iterator(link_node);
}

// 在pos处连接[first, last)的结点
template <typename T>
void List<T>::link_nodes(base_ptr pos, base_ptr first, base_ptr last) {
  pos->prev->next = first;
  first->prev = pos->prev;
  pos->prev = last;
  last->next = pos;
}

// 在头部连接[first, last)结点
template <typename T>
void List<T>::link_nodes_at_front(base_ptr first, base_ptr last) {
  first->prev = node_;
  last->next = node_->next;
  last->next->prev = last;
  node_->next = first;
}

// 在尾部连接[first, last)结点
template <typename T>
void List<T>::link_nodes_at_back(base_ptr first, base_ptr last) {
  last->next = node_;
  first->prev = node_->prev;
  first->prev->next = first;
  node_->prev = last;
}

// 容器与[frist, last)结点断开连接
template <typename T>
void List<T>::unlink_nodes(base_ptr first, base_ptr last) {
  first->prev->next = last->next;
  last->next->prev = first->prev;
}

// 用n个元素为容器赋值
template <typename T>
void List<T>::fill_assign(size_type n, const value_type& value) {
  auto i = begin();
  auto e = end();
  for (; n > 0 && i != e; --n, ++i) {
    *i = value;
  }
  if (n > 0) {
    insert(e, n, value);
  } else {
    erase(i, e);
  }
}

// 复制[f2, l2)为容器赋值
template <typename T>
template <typename Iter>
void List<T>::copy_assign(Iter f2, Iter l2) {
  auto f1 = begin();
  auto l1 = end();
  for (; f1 != l1 && f2 != l2; ++f1, ++f2) {
    *f1 = *f2;
  }
  if (f2 == l2) {
    erase(f1, l1);
  } else {
    insert(l1, f2, l2);
  }
}

// 在pos处插入n个元素
template <typename T>
typename List<T>::iterator List<T>::fill_insert(
    const_iterator pos, size_type n, const value_type& value) {
  iterator r(pos.node_);
  if (n != 0) {
    const auto add_size = n;
    auto node = create_node(value);
    node->prev = nullptr;
    r = iterator(node);
    iterator end = r;
    try {
      // 前面已经创建了一个结点，还需要n-1个
      for (--n; n > 0; --n, ++end) {
        auto next = create_node(value);
        end.node_->next = next->as_base();
        next->prev = end.node_;
      }
      size_ += add_size;
    } catch (...) {
      auto enode = end.node_;
      while (true) {
        auto prev = enode->prev;
        destroy(enode->as_node());
        if (prev == nullptr) {
          break;
        }
        enode = prev;
      }
      throw;
    }
    link_nodes(pos.node_, r.node_, end.node_);
  }
  return r;
}

// 在pos处插入[first, last)的元素
template <typename T>
template <typename Iter>
typename List<T>::iterator List<T>::copy_insert(const_iterator pos, size_type n, Iter first) {
  iterator r(pos.node_);
  if (n != 0) {
    const auto add_size = n;
    auto node = create_node(*first);
    node->prev = nullptr;
    r = iterator(node);
    iterator end = r;
    try {
      for (--n, ++first; n > 0; --n, ++first, ++end) {
        auto next = create_node(*first);
        end.node_->next = next->as_base();
        next->prev = end.node_;
      }
      size_ += add_size;
    } catch (...) {
      auto enode = end.node_;
      while (true) {
        auto prev = enode->prev;
        destroy_node(enode->as_node());
        if (prev == nullptr) {
          break;
        }
        enode = prev;
      }
      throw;
    }
    link_nodes(pos.node_, r.node_, end.node_);
  }
  return r;
}

// 对List进行归并排序，返回一个迭代器指向区间最小元素的位置
template <typename T>
template <typename Compared>
typename List<T>::iterator List<T>::list_sort(
    iterator f1, iterator l2, size_type n, Compared comp) {
  if (n < 2) {
    return f1;
  }

  if (n == 2) {
    if (comp(*--l2, *f1)) {
      auto ln = l2.node_;
      unlink_nodes(ln, ln);
      link_nodes(f1.node_, ln, ln);
      return l2;
    }
    return f1;
  }

  auto n2 = n / 2;
  auto l1 = f1;
  mystl::advance(l1, n2);
  auto result = f1 = list_sort(f1, l1, n2, comp);  // 前半段的最小位置
  auto f2 = l1 = list_sort(l1, l2, n - n2, comp);  // 后半段的最小位置

  // 把较小的一段区间移到前面
  if (comp(*f2, *f1)) {
    auto m = f2;
    ++m;
    for (; m != l2 && comp(*m, *f1); ++m) {
      ;
    }
    auto f = f2.node_;
    auto l = m.node_->prev;
    result = f2;
    l1 = f2 = m;
    unlink_nodes(f, l);
    m = f1;
    ++m;
    link_nodes(f1.node_, f, l);
    f1 = m;
  } else {
    ++f1;
  }

  // 合并两段有序区间
  while (f1 != l1 && f2 != l2) {
    if (comp(*f2, *f1)) {
      auto m = f2;
      ++m;
      for (; m != l2 && comp(*m, *f1); ++m) {
        ;
      }
      auto f = f2.node_;
      auto l = m.node_->prev;
      if (l1 == f2) {
        l1 = m;
      }
      f2 = m;
      unlink_nodes(f, l);
      ++m;
      link_nodes(f1.node_, f, l);
      f1 = m;
    } else {
      ++f1;
    }
  }
  return result;
}

// 重载比较操作符

template <typename T>
bool operator==(const List<T>& lhs, const List<T>& rhs) {
  auto f1 = lhs.cbegin();
  auto f2 = rhs.cbegin();
  auto l1 = lhs.cend();
  auto l2 = rhs.cend();
  for (; f1 != l1 && f2 != l2 && *f1 == *f2; ++f1, ++f2) {
    ;
  }
  return f1 == l1 && f2 == l2;
}

template <typename T>
bool operator<(const List<T>& lhs, const List<T>& rhs) {
  return mystl::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

template <typename T>
bool operator!=(const List<T>& lhs, const List<T>& rhs) {
  return !(lhs == rhs);
}

template <typename T>
bool operator>(const List<T>& lhs, const List<T>& rhs) {
  return rhs < lhs;
}

template <typename T>
bool operator<=(const List<T>& lhs, const List<T>& rhs) {
  return !(rhs < lhs);
}

template <typename T>
bool operator>=(const List<T>& lhs, const List<T>& rhs) {
  return !(lhs < rhs);
}

// 重载mystl的swap
template <typename T>
void swap(List<T>& lhs, List<T>& rhs) noexcept {
  lhs.swap(rhs);
}

}  // namespace mystl

#endif  // !MYTINYSTL_LIST_H_