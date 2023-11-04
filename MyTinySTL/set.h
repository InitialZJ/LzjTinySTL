#ifndef MYTINYSTL_SET_H_
#define MYTINYSTL_SET_H_

// 包含两个模板类 set 和 multiset
// set      : 集合，键值即实值，集合内元素会自动排序，键值不允许重复
// 对应书5.3节

// multiset : 集合，键值即实值，集合内元素会自动排序，键值允许重复
// 对应书5.5节

// notes:
//
// 异常保证：
// mystl::set<Key> / mystl::multiset<Key> 满足基本异常保证，对以下等函数做强异常安全保证：
//   * emplace
//   * emplace_hint
//   * insert

#include "rb_tree.h"

namespace mystl {

// 模板类set，键值不允许重复
// 参数一代表键值类型，参数二代表键值比较方式，缺省使用mystl::Less
template <typename Key, typename Compare = mystl::Less<Key>>
class Set {
 public:
  using key_type = Key;
  using value_type = Key;
  using key_compare = Compare;
  using value_compare = Compare;

 private:
  // 以mystl::RbTree作为底层机制
  using base_type = mystl::RbTree<value_type, key_compare>;
  base_type tree_;

 public:
  // 使用RbTree定义的型别
  using node_type = typename base_type::node_type;
  using pointer = typename base_type::const_pointer;
  using const_pointer = typename base_type::const_pointer;
  using reference = typename base_type::const_reference;
  using const_reference = typename base_type::const_reference;
  using iterator = typename base_type::const_iterator;
  using const_iterator = typename base_type::const_iterator;
  using reverse_iterator = typename base_type::const_reverse_iterator;
  using const_reverse_iterator = typename base_type::const_reverse_iterator;
  using size_type = typename base_type::size_type;
  using difference_type = typename base_type::difference_type;
  using allocator_type = typename base_type::allocator_type;

 public:
  // 构造、复制、移动函数
  Set() = default;

  template <typename InputIterator>
  Set(InputIterator first, InputIterator last) : tree_() {
    tree_.insert_unique(first, last);
  }
  Set(std::initializer_list<value_type> ilist) : tree_() {
    tree_.insert_unique(ilist.begin(), ilist.end());
  }

  Set(const Set& rhs) : tree_(rhs.tree_) {}
  Set(Set&& rhs) noexcept : tree_(mystl::move(rhs.tree_)) {}

  Set& operator=(const Set& rhs) {
    tree_ = rhs.tree_;
    return *this;
  }
  Set& operator=(Set&& rhs) {
    tree_ = mystl::move(rhs.tree_);
    return *this;
  }
  Set& operator=(std::initializer_list<value_type> ilist) {
    tree_.clear();
    tree_.insert_unique(ilist.begin(), ilist.end());
    return *this;
  }

  // 相关接口
  key_compare key_comp() const { return tree_.key_comp(); }
  value_compare value_comp() const { return tree_.key_comp(); }
  allocator_type get_allocator() const { return tree_.get_allocator(); }

  // 迭代器相关
  iterator begin() noexcept { return tree_.begin(); }
  const_iterator begin() const noexcept { return tree_.begin(); }
  iterator end() noexcept { return tree_.end(); }
  const_iterator end() const noexcept { return tree_.end(); }

  reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
  reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const noexcept { return cosnt_reverse_iterator(begin()); }

  const_iterator cbegin() const noexcept { return begin(); }
  const_iterator cend() const noexcept { return end(); }
  const_reverse_iterator crbegin() const noexcept { return rbegin(); }
  const_reverse_iterator crend() const noexcept { return rend(); }

  // 容量相关
  bool empty() const noexcept { return tree_.empty(); }
  size_type size() const noexcept { return tree_.size(); }
  size_type max_size() const noexcept { return tree_.max_size(); }

  // 插入删除操作
  template <typename... Args>
  pair<iterator, bool> emplace(Args&&... args) {
    return tree_.emplace_unique(mystl::forward<Args>(args)...);
  }

  template <typename... Args>
  iterator emplace_hint(iterator hint, Args&&... args) {
    return tree_.emplace_unique_use_hint(hint, mystl::forward<Args>(args)...);
  }

  pair<iterator, bool> insert(const value_type& value) { return tree_.insert_unique(value); }
  pair<iterator, bool> insert(value_type&& value) {
    return tree_.insert_unique(mystl::move(value));
  }

  iterator insert(iterator hint, const value_type& value) {
    return tree_.insert_unique(hint, value);
  }
  iterator insert(iterator hint, value_type&& value) {
    return tree_.insert_unique(hint, mystl::move(value));
  }

  template <typename InputIterator>
  void insert(InputIterator first, InputIterator last) {
    tree_.insert_unique(first, last);
  }

  void erase(iterator position) { tree_.erase(position); }
  size_type erase(const key_type& key) { return tree_.erase_unique(key); }
  void erase(iterator first, iterator last) { tree_.erase(first, last); }

  void clear() { tree_.clear(); }

  // set相关操作
  iterator find(const key_type& key) { return tree_.find(key); }
  const_iterator find(const key_type& key) const { return tree_.find(key); }

  size_type count(const key_type& key) const { return tree_.count_unique(key); }

  iterator lower_bound(const key_type& key) { return tree_.lower_bound(key); }
  const_iterator lower_bound(const key_type& key) const { return tree_.lower_bound(key); }

  iterator upper_bound(const key_type& key) { return tree_.upper_bound(key); }
  const_iterator upper_bound(const key_type& key) const { return tree_.upper_bound(key); }

  pair<iterator, iterator> equal_range(const key_type& key) {
    return tree_.equal_range_unique(key);
  }

  pair<const_iterator, const_iterator> equal_range(const key_type& key) const {
    return tree_.equal_range_multi(key);
  }

  void swap(Set& rhs) noexcept { tree_.swap(rhs.tree_); }

 public:
  friend bool operator==(const Set& lhs, const Set& rhs) { return lhs.tree_ == rhs.tree_; }
  friend bool operator<(const Set& lhs, const Set& rhs) { return lhs.tree_ < rhs.tree_; }
};

// 重载比较操作符
template <typename Key, typename Compare>
bool operator==(const Set<Key, Compare>& lhs, const Set<Key, Compare>& rhs) {
  return lhs == rhs;
}

template <typename Key, typename Compare>
bool operator<(const Set<Key, Compare>& lhs, const Set<Key, Compare>& rhs) {
  return lhs < rhs;
}

template <typename Key, typename Compare>
bool operator!=(const Set<Key, Compare>& lhs, const Set<Key, Compare>& rhs) {
  return !(lhs == rhs);
}

template <typename Key, typename Compare>
bool operator>(const Set<Key, Compare>& lhs, const Set<Key, Compare>& rhs) {
  return rhs < lhs;
}

template <typename Key, typename Compare>
bool operator<=(const Set<Key, Compare>& lhs, const Set<Key, Compare>& rhs) {
  return !(rhs < lhs);
}

template <typename Key, typename Compare>
bool operator>=(const Set<Key, Compare>& lhs, const Set<Key, Compare>& rhs) {
  return !(lhs < rhs);
}

// 重载mystl的swap
template <typename Key, typename Compare>
void swap(Set<Key, Compare>& lhs, Set<Key, Compare>& rhs) noexcept {
  lhs.swap(rhs);
}

// 模板类multiset，键值允许重复
// 参数一代表键值类型，参数二代表键值比较方式，缺省使用mystl::Less
template <typename Key, typename Compare = mystl::Less<Key>>
class MultiSet {
 public:
  using key_type = Key;
  using value_type = Key;
  using key_compare = Compare;
  using value_compare = Compare;

 private:
  // 以mystl::RbTree作为底层机制
  using base_type = mystl::RbTree<value_type, key_compare>;
  base_type tree_;

 public:
  // 使用RbTree定义的型别
  using node_type = typename base_type::node_type;
  using pointer = typename base_type::const_pointer;
  using const_pointer = typename base_type::const_pointer;
  using reference = typename base_type::const_reference;
  using const_reference = typename base_type::const_reference;
  using iterator = typename base_type::const_iterator;
  using const_iterator = typename base_type::const_iterator;
  using reverse_iterator = typename base_type::const_reverse_iterator;
  using const_reverse_iterator = typename base_type::const_reverse_iterator;
  using size_type = typename base_type::size_type;
  using difference_type = typename base_type::difference_type;
  using allocator_type = typename base_type::allocator_type;

 public:
  // 构造、复制、移动函数
  MultiSet() = default;

  template <typename InputIterator>
  MultiSet(InputIterator first, InputIterator last) : tree_() {
    tree_.insert_multi(first, last);
  }
  MultiSet(std::initializer_list<value_type> ilist) : tree_() {
    tree_.insert_multi(ilist.begin(), ilist.end());
  }

  MultiSet(const MultiSet& rhs) : tree_(rhs.tree_) {}
  MultiSet(MultiSet&& rhs) noexcept : tree_(mystl::move(rhs.tree_)) {}

  MultiSet& operator=(const MultiSet& rhs) {
    tree_ = rhs.tree_;
    return *this;
  }
  MultiSet& operator=(MultiSet&& rhs) {
    tree_ = mystl::move(rhs.tree_);
    return *this;
  }
  MultiSet& operator=(std::initializer_list<value_type> ilist) {
    tree_.clear();
    tree_.insert_multi(ilist.begin(), ilist.end());
    return *this;
  }

  // 相关接口
  key_compare key_comp() const { return tree_.key_comp(); }
  value_compare value_comp() const { return tree_.key_comp(); }
  allocator_type get_allocator() const { return tree_.get_allocator(); }

  // 迭代器相关
  iterator begin() noexcept { return tree_.begin(); }
  const_iterator begin() const noexcept { return tree_.begin(); }
  iterator end() noexcept { return tree_.end(); }
  const_iterator end() const noexcept { return tree_.end(); }

  reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
  reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const noexcept { return cosnt_reverse_iterator(begin()); }

  const_iterator cbegin() const noexcept { return begin(); }
  const_iterator cend() const noexcept { return end(); }
  const_reverse_iterator crbegin() const noexcept { return rbegin(); }
  const_reverse_iterator crend() const noexcept { return rend(); }

  // 容量相关
  bool empty() const noexcept { return tree_.empty(); }
  size_type size() const noexcept { return tree_.size(); }
  size_type max_size() const noexcept { return tree_.max_size(); }

  // 插入删除操作
  template <typename... Args>
  iterator emplace(Args&&... args) {
    return tree_.emplace_multi(mystl::forward<Args>(args)...);
  }

  template <typename... Args>
  iterator emplace_hint(iterator hint, Args&&... args) {
    return tree_.emplace_multi_use_hint(hint, mystl::forward<Args>(args)...);
  }

  iterator insert(const value_type& value) { return tree_.insert_multi(value); }
  iterator insert(value_type&& value) { return tree_.insert_multi(mystl::move(value)); }

  iterator insert(iterator hint, const value_type& value) {
    return tree_.insert_multi(hint, value);
  }
  iterator insert(iterator hint, value_type&& value) {
    return tree_.insert_multi(hint, mystl::move(value));
  }

  template <typename InputIterator>
  void insert(InputIterator first, InputIterator last) {
    tree_.insert_multi(first, last);
  }

  void erase(iterator position) { tree_.erase(position); }
  size_type erase(const key_type& key) { return tree_.erase_multi(key); }
  void erase(iterator first, iterator last) { tree_.erase(first, last); }

  void clear() { tree_.clear(); }

  // set相关操作
  iterator find(const key_type& key) { return tree_.find(key); }
  const_iterator find(const key_type& key) const { return tree_.find(key); }

  size_type count(const key_type& key) const { return tree_.count_multi(key); }

  iterator lower_bound(const key_type& key) { return tree_.lower_bound(key); }
  const_iterator lower_bound(const key_type& key) const { return tree_.lower_bound(key); }

  iterator upper_bound(const key_type& key) { return tree_.upper_bound(key); }
  const_iterator upper_bound(const key_type& key) const { return tree_.upper_bound(key); }

  pair<iterator, iterator> equal_range(const key_type& key) { return tree_.equal_range_multi(key); }

  pair<const_iterator, const_iterator> equal_range(const key_type& key) const {
    return tree_.equal_range_multi(key);
  }

  void swap(MultiSet& rhs) noexcept { tree_.swap(rhs.tree_); }

 public:
  friend bool operator==(const MultiSet& lhs, const MultiSet& rhs) {
    return lhs.tree_ == rhs.tree_;
  }
  friend bool operator<(const MultiSet& lhs, const MultiSet& rhs) { return lhs.tree_ < rhs.tree_; }
};

// 重载比较操作符
template <typename Key, typename Compare>
bool operator==(const MultiSet<Key, Compare>& lhs, const MultiSet<Key, Compare>& rhs) {
  return lhs == rhs;
}

template <typename Key, typename Compare>
bool operator<(const MultiSet<Key, Compare>& lhs, const MultiSet<Key, Compare>& rhs) {
  return lhs < rhs;
}

template <typename Key, typename Compare>
bool operator!=(const MultiSet<Key, Compare>& lhs, const MultiSet<Key, Compare>& rhs) {
  return !(lhs == rhs);
}

template <typename Key, typename Compare>
bool operator>(const MultiSet<Key, Compare>& lhs, const MultiSet<Key, Compare>& rhs) {
  return rhs < lhs;
}

template <typename Key, typename Compare>
bool operator<=(const MultiSet<Key, Compare>& lhs, const MultiSet<Key, Compare>& rhs) {
  return !(rhs < lhs);
}

template <typename Key, typename Compare>
bool operator>=(const MultiSet<Key, Compare>& lhs, const MultiSet<Key, Compare>& rhs) {
  return !(lhs < rhs);
}

// 重载mystl的swap
template <typename Key, typename Compare>
void swap(MultiSet<Key, Compare>& lhs, MultiSet<Key, Compare>& rhs) noexcept {
  lhs.swap(rhs);
}

}  // namespace mystl

#endif  // ! MYTINYSTL_SET_H_
