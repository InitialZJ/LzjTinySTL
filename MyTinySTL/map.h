#ifndef MYTINYSTL_MAP_H_
#define MYTINYSTL_MAP_H_

// 这个头文件包含了两个模板类 map 和 multimap
// map      : 映射，元素具有键值和实值，会根据键值大小自动排序，键值不允许重复
// 对应书5.4节

// multimap : 映射，元素具有键值和实值，会根据键值大小自动排序，键值允许重复
// 对应书5.6节

// notes:
//
// 异常保证：
// mystl::map<Key, T> / mystl::multimap<Key, T> 满足基本异常保证，对以下等函数做强异常安全保证：
//   * emplace
//   * emplace_hint
//   * insert

#include <initializer_list>

#include "iterator.h"
#include "rb_tree.h"
#include "util.h"

namespace stl {
// 模板类map
// 参数一代表键值类型，参数二代表实值类型，参数三代表键值的比较方式，缺省实用mystl::less
template <typename Key, typename T, typename Compare = mystl::Less<Key>>
class Map {
 public:
  using key_type = Key;
  using mapped_type = T;
  using value_type = mystl::pair<const Key, T>;
  using key_compare = Compare;

  // 定义一个functor，用来进行元素比较
  class ValueCompare : public mystl::BinaryFunction<value_type, value_type, bool> {
    friend class Map<Key, T, Compare>;

   private:
    Compare comp_;
    ValueCompare(Compare c) : comp_(c) {}

   public:
    bool operator()(const value_type& lhs, const value_type& rhs) const {
      return comp_(lhs.first, rhs.first);
    }
  };

 private:
  using base_type = mystl::RbTree<value_type, key_compare>;
  base_type tree_;

 public:
  using node_type = typename base_type::node_type;
  using pointer = typename base_type::pointer;
  using const_pointer = typename base_type::const_pointer;
  using reference = typename base_type::reference;
  using const_reference = typename base_type::const_reference;
  using iterator = typename base_type::iterator;
  using const_iterator = typename base_type::const_iterator;
  using reverse_iterator = typename base_type::reverse_iterator;
  using const_reverse_iterator = typename base_type::const_reverse_iterator;
  using size_type = typename base_type::size_type;
  using difference_type = typename base_type::difference_type;
  using allocator_type = typename base_type::allocator_type;

 public:
  Map() = default;

  template <typename InputIterator>
  Map(InputIterator first, InputIterator last) : tree_() {
    tree_.insert_unique(first, last);
  }

  Map(std::initializer_list<value_type> ilist) : tree_() {
    tree_.insert_unique(ilist.begin(), ilist.end());
  }

  Map(const Map& rhs) : tree_(rhs.tree_) {}
  Map(Map&& rhs) : tree_(mystl::move(rhs.tree_)) {}

  Map& operator=(const Map& rhs) {
    tree_ = rhs.tree_;
    return *this;
  }
  Map& operator=(Map&& rhs) {
    tree_ = mystl::move(rhs.tree_);
    return *this;
  }

  Map& operator=(std::initializer_list<value_type> ilist) {
    tree_.clear();
    tree_.insert_unique(ilist.begin(), ilist.end());
    return *this;
  }

  key_compare key_comp() const { return tree_.key_comp(); }
  ValueCompare value_comp() const { return ValueCompare(tree_.key_comp()); }
  allocator_type get_allocator() const { return tree_.get_allocator(); }

  iterator begin() noexcept { return tree_.begin(); }
  const_iterator begin() const noexcept { return tree_.begin(); }
  iterator end() noexcept { return tree_.end(); }
  const_iterator end() const noexcept { return tree_.end(); }

  reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
  reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

  const_iterator cbegin() const noexcept { return begin(); }
  const_iterator cend() const noexcept { return end(); }
  const_reverse_iterator crbegin() const noexcept { return rbegin(); }
  const_reverse_iterator crend() const noexcept { return rend(); }

  bool empty() const noexcept { return tree_.empty(); }
  size_type size() const noexcept { return tree_.size(); }
  size_type max_size() const noexcept { return tree_.max_size(); }

  // 访问元素相关

  // 若键值不存在，at会抛出异常
  mapped_type& at(const key_type& key) {
    iterator it = lower_bound(key);
    // it->first >= key
    THROW_OUT_OF_RANGE_IF(
        it == end() || key_comp()(it->first, key), "map<Key, T> no such element exists");
    return it->second;
  }
  const mapped_type& at(const key_type& key) const {
    const_iterator it = lower_bound(key);
    // it->first >= key
    THROW_OUT_OF_RANGE_IF(
        it == end() || key_comp()(it->first, key), "map<Key, T> no such element exists");
    return it->second;
  }

  mapped_type& operator[](const key_type& key) {
    iterator it = lower_bound(key);
    if (it == end() || key_comp()(key, it->first)) {
      it = emplace_hint(it, key, T{});
    }
    return it->second;
  }
  mapped_type& operator[](key_type&& key) {
    iterator it = lower_bound(key);
    if (it == end() || key_comp()(key, it->first)) {
      it = emplace_hint(it, std::move(key), T{});
    }
    return it->second;
  }

  // 插入删除相关
  template <typename... Args>
  mystl::pair<iterator, bool> emplace(Args&&... args) {
    return tree_.emplace_unique(mystl::forward<Args>(args)...);
  }

  template <typename... Args>
  iterator emplace_hint(iterator hint, Args&&... args) {
    return tree_.emplace_unique_use_hint(hint, mystl::forward<Args>(args)...);
  }

  mystl::pair<iterator, bool> insert(const value_type& value) { return tree_.insert_unique(value); }
  mystl::pair<iterator, bool> insert(value_type&& value) {
    return tree_.insert_unique(std::move(value));
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

  // map相关操作
  iterator find(const key_type& key) { return tree_.find(key); }
  const_iterator find(const key_type& key) const { return tree_.find(key); }

  size_type count(const key_type& key) const { return tree_.count_unique(key); }

  iterator lower_bound(const key_type& key) { return tree_.lower_bound(key); }
  const_iterator lower_bound(const key_type& key) const { return tree_.lower_bound(key); }

  iterator upper_bound(const key_type& key) { return tree_.upper_bound(key); }
  const_iterator upper_bound(const key_type& key) const { return tree_.upper_bound(key); }

  mystl::pair<iterator, iterator> equal_range(const key_type& key) {
    return tree_.equal_range_unique(key);
  }

  mystl::pair<const_iterator, const_iterator> equal_range(const key_type& key) const {
    return tree_.equal_range_unique(key);
  }

  void swap(Map& rhs) noexcept { tree_.swap(rhs.tree_); }

 public:
  friend bool operator==(const Map& lhs, const Map& rhs) { return lhs.tree_ == rhs.tree_; }
  friend bool operator<(const Map& lhs, const Map& rhs) { return lhs.tree_ < rhs.tree_; }
};

// 重载比较操作符
template <class Key, class T, class Compare>
bool operator==(const Map<Key, T, Compare>& lhs, const Map<Key, T, Compare>& rhs) {
  return lhs == rhs;
}

template <class Key, class T, class Compare>
bool operator<(const Map<Key, T, Compare>& lhs, const Map<Key, T, Compare>& rhs) {
  return lhs < rhs;
}

template <class Key, class T, class Compare>
bool operator!=(const Map<Key, T, Compare>& lhs, const Map<Key, T, Compare>& rhs) {
  return !(lhs == rhs);
}

template <class Key, class T, class Compare>
bool operator>(const Map<Key, T, Compare>& lhs, const Map<Key, T, Compare>& rhs) {
  return rhs < lhs;
}

template <class Key, class T, class Compare>
bool operator<=(const Map<Key, T, Compare>& lhs, const Map<Key, T, Compare>& rhs) {
  return !(rhs < lhs);
}

template <class Key, class T, class Compare>
bool operator>=(const Map<Key, T, Compare>& lhs, const Map<Key, T, Compare>& rhs) {
  return !(lhs < rhs);
}

// 重载 mystl 的 swap
template <class Key, class T, class Compare>
void swap(Map<Key, T, Compare>& lhs, Map<Key, T, Compare>& rhs) noexcept {
  lhs.swap(rhs);
}

// 模板类multimap
// 参数一代表键值类型，参数二代表实值类型，参数三代表键值的比较方式，缺省实用mystl::less
template <typename Key, typename T, typename Compare = mystl::Less<Key>>
class MultiMap {
 public:
  using key_type = Key;
  using mapped_type = T;
  using value_type = mystl::pair<const Key, T>;
  using key_compare = Compare;

  // 定义一个functor，用来进行元素比较
  class ValueCompare : public mystl::BinaryFunction<value_type, value_type, bool> {
    friend class Map<Key, T, Compare>;

   private:
    Compare comp_;
    ValueCompare(Compare c) : comp_(c) {}

   public:
    bool operator()(const value_type& lhs, const value_type& rhs) const {
      return comp_(lhs.first, rhs.first);
    }
  };

 private:
  using base_type = mystl::RbTree<value_type, key_compare>;
  base_type tree_;

 public:
  using node_type = typename base_type::node_type;
  using pointer = typename base_type::pointer;
  using const_pointer = typename base_type::const_pointer;
  using reference = typename base_type::reference;
  using const_reference = typename base_type::const_reference;
  using iterator = typename base_type::iterator;
  using const_iterator = typename base_type::const_iterator;
  using reverse_iterator = typename base_type::reverse_iterator;
  using const_reverse_iterator = typename base_type::const_reverse_iterator;
  using size_type = typename base_type::size_type;
  using difference_type = typename base_type::difference_type;
  using allocator_type = typename base_type::allocator_type;

 public:
  MultiMap() = default;

  template <typename InputIterator>
  MultiMap(InputIterator first, InputIterator last) : tree_() {
    tree_.insert_multi(first, last);
  }

  MultiMap(std::initializer_list<value_type> ilist) : tree_() {
    tree_.insert_multi(ilist.begin(), ilist.end());
  }

  MultiMap(const MultiMap& rhs) : tree_(rhs.tree_) {}
  MultiMap(MultiMap&& rhs) : tree_(mystl::move(rhs.tree_)) {}

  MultiMap& operator=(const MultiMap& rhs) {
    tree_ = rhs.tree_;
    return *this;
  }
  MultiMap& operator=(MultiMap&& rhs) {
    tree_ = mystl::move(rhs.tree_);
    return *this;
  }

  MultiMap& operator=(std::initializer_list<value_type> ilist) {
    tree_.clear();
    tree_.insert_multi(ilist.begin(), ilist.end());
    return *this;
  }

  key_compare key_comp() const { return tree_.key_comp(); }
  ValueCompare value_comp() const { return ValueCompare(tree_.key_comp()); }
  allocator_type get_allocator() const { return tree_.get_allocator(); }

  iterator begin() noexcept { return tree_.begin(); }
  const_iterator begin() const noexcept { return tree_.begin(); }
  iterator end() noexcept { return tree_.end(); }
  const_iterator end() const noexcept { return tree_.end(); }

  reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
  reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

  const_iterator cbegin() const noexcept { return begin(); }
  const_iterator cend() const noexcept { return end(); }
  const_reverse_iterator crbegin() const noexcept { return rbegin(); }
  const_reverse_iterator crend() const noexcept { return rend(); }

  bool empty() const noexcept { return tree_.empty(); }
  size_type size() const noexcept { return tree_.size(); }
  size_type max_size() const noexcept { return tree_.max_size(); }

  // 插入删除相关
  template <typename... Args>
  mystl::pair<iterator, bool> emplace(Args&&... args) {
    return tree_.emplace_multi(mystl::forward<Args>(args)...);
  }

  template <typename... Args>
  iterator emplace_hint(iterator hint, Args&&... args) {
    return tree_.emplace_multi_use_hint(hint, mystl::forward<Args>(args)...);
  }

  mystl::pair<iterator, bool> insert(const value_type& value) { return tree_.insert_multi(value); }
  mystl::pair<iterator, bool> insert(value_type&& value) {
    return tree_.insert_multi(std::move(value));
  }

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

  // map相关操作
  iterator find(const key_type& key) { return tree_.find(key); }
  const_iterator find(const key_type& key) const { return tree_.find(key); }

  size_type count(const key_type& key) const { return tree_.count_multi(key); }

  iterator lower_bound(const key_type& key) { return tree_.lower_bound(key); }
  const_iterator lower_bound(const key_type& key) const { return tree_.lower_bound(key); }

  iterator upper_bound(const key_type& key) { return tree_.upper_bound(key); }
  const_iterator upper_bound(const key_type& key) const { return tree_.upper_bound(key); }

  mystl::pair<iterator, iterator> equal_range(const key_type& key) {
    return tree_.equal_range_multi(key);
  }

  mystl::pair<const_iterator, const_iterator> equal_range(const key_type& key) const {
    return tree_.equal_range_multi(key);
  }

  void swap(MultiMap& rhs) noexcept { tree_.swap(rhs.tree_); }

 public:
  friend bool operator==(const MultiMap& lhs, const MultiMap& rhs) {
    return lhs.tree_ == rhs.tree_;
  }
  friend bool operator<(const MultiMap& lhs, const MultiMap& rhs) { return lhs.tree_ < rhs.tree_; }
};

// 重载比较操作符
template <class Key, class T, class Compare>
bool operator==(const MultiMap<Key, T, Compare>& lhs, const MultiMap<Key, T, Compare>& rhs) {
  return lhs == rhs;
}

template <class Key, class T, class Compare>
bool operator<(const MultiMap<Key, T, Compare>& lhs, const MultiMap<Key, T, Compare>& rhs) {
  return lhs < rhs;
}

template <class Key, class T, class Compare>
bool operator!=(const MultiMap<Key, T, Compare>& lhs, const MultiMap<Key, T, Compare>& rhs) {
  return !(lhs == rhs);
}

template <class Key, class T, class Compare>
bool operator>(const MultiMap<Key, T, Compare>& lhs, const MultiMap<Key, T, Compare>& rhs) {
  return rhs < lhs;
}

template <class Key, class T, class Compare>
bool operator<=(const MultiMap<Key, T, Compare>& lhs, const MultiMap<Key, T, Compare>& rhs) {
  return !(rhs < lhs);
}

template <class Key, class T, class Compare>
bool operator>=(const MultiMap<Key, T, Compare>& lhs, const MultiMap<Key, T, Compare>& rhs) {
  return !(lhs < rhs);
}

// 重载 mystl 的 swap
template <class Key, class T, class Compare>
void swap(MultiMap<Key, T, Compare>& lhs, MultiMap<Key, T, Compare>& rhs) noexcept {
  lhs.swap(rhs);
}

}  // namespace stl
