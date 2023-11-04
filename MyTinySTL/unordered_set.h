#ifndef MYTINYSTL_UNORDERED_SET_H_
#define MYTINYSTL_UNORDERED_SET_H_

// 这个头文件包含两个模板类 unordered_set 和 unordered_multiset
// 功能与用法与 set 和 multiset 类似，不同的是使用 hashtable
// 作为底层实现机制，容器中的元素不会自动排序

// 对应书5.8、5.10节

// notes:
//
// 异常保证：
// mystl::unordered_set<Key> / mystl::unordered_multiset<Key>
// 满足基本异常保证，对以下等函数做强异常安全保证：
//   * emplace
//   * emplace_hint
//   * insert

#include <initializer_list>

#include "algobase.h"
#include "hashtable.h"
#include "iterator.h"

namespace mystl {

// 模板类unordered_set，键值不允许重复
// 参数一代表键值类型，参数二代表哈希函数，缺省使用mystl::hash，
// 参数三代表键值比较方式，缺省使用mystl::equal_to
template <typename Key, typename Hash = mystl::Hash<Key>, typename KeyEqual = mystl::EqualTo<Key>>
class UnorderedSet {
 private:
  // 使用hashtable作为底层机制
  using base_type = Hashtable<Key, Hash, KeyEqual>;
  base_type ht_;

 public:
  // 使用hashtable的型别
  using allocator_type = typename base_type::allocator_type;
  using key_type = typename base_type::key_type;
  using value_type = typename base_type::value_type;
  using hasher = typename base_type::hasher;
  using key_equal = typename base_type::key_equal;

  using size_type = typename base_type::size_type;
  using difference_type = typename base_type::difference_type;
  using pointer = typename base_type::pointer;
  using const_pointer = typename base_type::const_pointer;
  using reference = typename base_type::reference;
  using const_reference = typename base_type::const_reference;

  using iterator = typename base_type::const_iterator;
  using const_iterator = typename base_type::const_iterator;
  using local_iterator = typename base_type::const_local_iterator;
  using const_local_iterator = typename base_type::const_local_iterator;

  allocator_type get_allocator() const { return ht_.get_allocator(); }

 public:
  UnorderedSet() : ht_(100, Hash(), KeyEqual()) {}

  explicit UnorderedSet(
      size_type bucket_count, const Hash& hash = Hash(), const KeyEqual& equal = KeyEqual())
      : ht_(bucket_count, hash, equal) {}

  template <typename InputIterator>
  UnorderedSet(
      InputIterator first,
      InputIterator last,
      const size_type bucket_count = 100,
      const Hash& hash = Hash(),
      const KeyEqual& equal = KeyEqual())
      : ht_(mystl::max(bucket_count, static_cast<size_type>(mystl::distance(first, last))),
            hash,
            equal) {
    for (; first != last; ++first) {
      ht_.insert_unique_noresize(*first);
    }
  }

  UnorderedSet(
      std::initializer_list<value_type> ilist,
      const size_type bucket_count = 100,
      const Hash& hash = Hash(),
      const KeyEqual& equal = KeyEqual())
      : ht_(mystl::max(bucket_count, static_cast<size_type>(ilist.size())), hash, equal) {
    for (auto first = ilist.begin(), last = ilist.end(); first != last; ++first) {
      ht_.insert_unique_noresize(*first);
    }
  }

  UnorderedSet(const UnorderedSet& rhs) : ht_(rhs.ht_) {}
  UnorderedSet(UnorderedSet&& rhs) noexcept : ht_(mystl::move(rhs.ht_)) {}

  UnorderedSet& operator=(const UnorderedSet& rhs) {
    ht_ = rhs.ht_;
    return *this;
  }
  UnorderedSet& operator=(UnorderedSet&& rhs) {
    ht_ = mystl::move(rhs.ht_);
    return *this;
  }

  UnorderedSet& operator=(std::initializer_list<value_type> ilist) {
    ht_.clear();
    ht_.reserve(ilist.size());
    for (auto first = ilist.begin(), last = ilist.end(); first != last; ++first) {
      ht_.insert_unique_noresize(*first);
    }
    return *this;
  }

  ~UnorderedSet() = default;

  // 迭代器相关
  iterator begin() noexcept { return ht_.begin(); }
  const_iterator begin() const noexcept { return ht_.begin(); }
  iterator end() noexcept { return ht_.end(); }
  const_iterator end() const noexcept { return ht_.end(); }

  const_iterator cbegin() const noexcept { return ht_.cbegin(); }
  const_iterator cend() const noexcept { return ht_.cend(); }

  // 容量相关
  bool empty() const noexcept { return ht_.empty(); }
  size_type size() const noexcept { return ht_.size(); }
  size_type max_size() const noexcept { return ht_.max_size(); }

  // 修改容器相关操作
  template <typename... Args>
  pair<iterator, bool> emplace(Args&&... args) {
    return ht_.emplace_unique(mystl::forward<Args>(args)...);
  }

  template <typename... Args>
  iterator emplace_hint(const_iterator hint, Args&&... args) {
    return ht_.emplace_unique_use_hint(hint, mystl::forward<Args>(args)...);
  }

  pair<iterator, bool> insert(const value_type& value) { return ht_.insert_unique(value); }
  pair<iterator, bool> insert(value_type&& value) { return ht_.emplace_unique(mystl::move(value)); }

  iterator insert(const_iterator hint, const value_type& value) {
    return ht_.insert_unique_use_hint(hint, value);
  }
  iterator insert(const_iterator hint, value_type&& value) {
    return ht_.emplace_unique_use_hint(hint, mystl::move(value));
  }

  template <typename InputIterator>
  void insert(InputIterator first, InputIterator last) {
    ht_.insert_unique(first, last);
  }

  void erase(iterator it) { ht_.erase(it); }
  void erase(iterator first, iterator last) { ht_.erase(first, last); }

  size_type erase(const key_type& key) { return ht_.erase_unique(key); }

  void clear() { ht_.clear(); }

  void swap(UnorderedSet& other) noexcept { ht_.swap(other.ht_); }

  size_type count(const key_type& key) const { return ht_.count(key); }

  iterator find(const key_type& key) { return ht_.find(key); }
  const_iterator find(const key_type& key) const { return ht_.find(key); }

  pair<iterator, iterator> equal_range(const key_type& key) { return ht_.equal_range_unique(key); }
  pair<const_iterator, const_iterator> equal_range(const key_type& key) const {
    return ht_.equal_range_unique(key);
  }

  local_iterator begin(size_type n) noexcept { return ht_.begin(n); }
  const_local_iterator begin(size_type n) const noexcept { return ht_.begin(n); }
  const_local_iterator cbegin(size_type n) const noexcept { return ht_.cbegin(n); }

  local_iterator end(size_type n) noexcept { return ht_.end(n); }
  const_local_iterator end(size_type n) const noexcept { return ht_.end(n); }
  const_local_iterator cend(size_type n) const noexcept { return ht_.cend(n); }

  size_type bucket_count() const noexcept { return ht_.bucket_count(); }
  size_type max_bucket_count() const noexcept { return ht_.max_bucket_count(); }

  size_type bucket_size(size_type n) const noexcept { return ht_.bucket_size(n); }
  size_type bucket(const key_type& key) const { return ht_.bucket(key); }

  // hash policy
  float load_factor() const noexcept { return ht_.load_factor(); }

  float max_load_factor() const noexcept { return ht_.max_load_factor(); }
  void max_load_factor(float ml) { ht_.max_load_factor(ml); }

  void rehash(size_type count) { ht_.rehash(count); }
  void reserve(size_type count) { ht_.reserve(count); }

  hasher hash_fcn() const { return ht_.hash_fcn(); }
  key_equal key_eq() const { return ht_.key_eq(); }

 public:
  friend bool operator==(const UnorderedSet& lhs, const UnorderedSet& rhs) {
    return lhs.ht_.equal_to_unique(rhs.ht_);
  }
  friend bool operator!=(const UnorderedSet& lhs, const UnorderedSet& rhs) {
    return !lhs.ht_.equal_to_unique(rhs.ht_);
  }
};

template <typename Key, typename Hash, typename KeyEqual>
bool operator==(
    const UnorderedSet<Key, Hash, KeyEqual>& lhs, const UnorderedSet<Key, Hash, KeyEqual>& rhs) {
  return lhs == rhs;
}

template <typename Key, typename Hash, typename KeyEqual>
bool operator!=(
    const UnorderedSet<Key, Hash, KeyEqual>& lhs, const UnorderedSet<Key, Hash, KeyEqual>& rhs) {
  return lhs != rhs;
}

template <typename Key, typename Hash, typename KeyEqual>
void swap(
    const UnorderedSet<Key, Hash, KeyEqual>& lhs, const UnorderedSet<Key, Hash, KeyEqual>& rhs) {
  lhs.swap(rhs);
}

// 模板类unordered_multiset，键值允许重复
// 参数一代表键值类型，参数二代表哈希函数，缺省使用mystl::hash，
// 参数三代表键值比较方式，缺省使用mystl::equal_to
template <typename Key, typename Hash = mystl::Hash<Key>, typename KeyEqual = mystl::EqualTo<Key>>
class UnorderedMultiSet {
 private:
  // 使用hashtable作为底层机制
  using base_type = Hashtable<Key, Hash, KeyEqual>;
  base_type ht_;

 public:
  // 使用hashtable的型别
  using allocator_type = typename base_type::allocator_type;
  using key_type = typename base_type::key_type;
  using value_type = typename base_type::value_type;
  using hasher = typename base_type::hasher;
  using key_equal = typename base_type::key_equal;

  using size_type = typename base_type::size_type;
  using difference_type = typename base_type::difference_type;
  using pointer = typename base_type::pointer;
  using const_pointer = typename base_type::const_pointer;
  using reference = typename base_type::reference;
  using const_reference = typename base_type::const_reference;

  using iterator = typename base_type::const_iterator;
  using const_iterator = typename base_type::const_iterator;
  using local_iterator = typename base_type::const_local_iterator;
  using const_local_iterator = typename base_type::const_local_iterator;

  allocator_type get_allocator() const { return ht_.get_allocator(); }

 public:
  UnorderedMultiSet() : ht_(100, Hash(), KeyEqual()) {}

  explicit UnorderedMultiSet(
      size_type bucket_count, const Hash& hash = Hash(), const KeyEqual& equal = KeyEqual())
      : ht_(bucket_count, hash, equal) {}

  template <typename InputIterator>
  UnorderedMultiSet(
      InputIterator first,
      InputIterator last,
      const size_type bucket_count = 100,
      const Hash& hash = Hash(),
      const KeyEqual& equal = KeyEqual())
      : ht_(mystl::max(bucket_count, static_cast<size_type>(mystl::distance(first, last))),
            hash,
            equal) {
    for (; first != last; ++first) {
      ht_.insert_multi_noresize(*first);
    }
  }

  UnorderedMultiSet(
      std::initializer_list<value_type> ilist,
      const size_type bucket_count = 100,
      const Hash& hash = Hash(),
      const KeyEqual& equal = KeyEqual())
      : ht_(mystl::max(bucket_count, static_cast<size_type>(ilist.size())), hash, equal) {
    for (auto first = ilist.begin(), last = ilist.end(); first != last; ++first) {
      ht_.insert_multi_noresize(*first);
    }
  }

  UnorderedMultiSet(const UnorderedMultiSet& rhs) : ht_(rhs.ht_) {}
  UnorderedMultiSet(UnorderedMultiSet&& rhs) noexcept : ht_(mystl::move(rhs.ht_)) {}

  UnorderedMultiSet& operator=(const UnorderedMultiSet& rhs) {
    ht_ = rhs.ht_;
    return *this;
  }
  UnorderedMultiSet& operator=(UnorderedMultiSet&& rhs) {
    ht_ = mystl::move(rhs.ht_);
    return *this;
  }

  UnorderedMultiSet& operator=(std::initializer_list<value_type> ilist) {
    ht_.clear();
    ht_.reserve(ilist.size());
    for (auto first = ilist.begin(), last = ilist.end(); first != last; ++first) {
      ht_.insert_multi_noresize(*first);
    }
    return *this;
  }

  ~UnorderedMultiSet() = default;

  // 迭代器相关
  iterator begin() noexcept { return ht_.begin(); }
  const_iterator begin() const noexcept { return ht_.begin(); }
  iterator end() noexcept { return ht_.end(); }
  const_iterator end() const noexcept { return ht_.end(); }

  const_iterator cbegin() const noexcept { return ht_.cbegin(); }
  const_iterator cend() const noexcept { return ht_.cend(); }

  // 容量相关
  bool empty() const noexcept { return ht_.empty(); }
  size_type size() const noexcept { return ht_.size(); }
  size_type max_size() const noexcept { return ht_.max_size(); }

  // 修改容器相关操作
  template <typename... Args>
  iterator emplace(Args&&... args) {
    return ht_.emplace_multi(mystl::forward<Args>(args)...);
  }

  template <typename... Args>
  iterator emplace_hint(const_iterator hint, Args&&... args) {
    return ht_.emplace_multi_use_hint(hint, mystl::forward<Args>(args)...);
  }

  iterator insert(const value_type& value) { return ht_.insert_multi(value); }
  iterator insert(value_type&& value) { return ht_.emplace_multi(mystl::move(value)); }

  iterator insert(const_iterator hint, const value_type& value) {
    return ht_.insert_multi_use_hint(hint, value);
  }
  iterator insert(const_iterator hint, value_type&& value) {
    return ht_.emplace_multi_use_hint(hint, mystl::move(value));
  }

  template <typename InputIterator>
  void insert(InputIterator first, InputIterator last) {
    ht_.insert_multi(first, last);
  }

  void erase(iterator it) { ht_.erase(it); }
  void erase(iterator first, iterator last) { ht_.erase(first, last); }

  size_type erase(const key_type& key) { return ht_.erase_multi(key); }

  void clear() { ht_.clear(); }

  void swap(UnorderedMultiSet& other) noexcept { ht_.swap(other.ht_); }

  size_type count(const key_type& key) const { return ht_.count(key); }

  iterator find(const key_type& key) { return ht_.find(key); }
  const_iterator find(const key_type& key) const { return ht_.find(key); }

  pair<iterator, iterator> equal_range(const key_type& key) { return ht_.equal_range_multi(key); }
  pair<const_iterator, const_iterator> equal_range(const key_type& key) const {
    return ht_.equal_range_multi(key);
  }

  local_iterator begin(size_type n) noexcept { return ht_.begin(n); }
  const_local_iterator begin(size_type n) const noexcept { return ht_.begin(n); }
  const_local_iterator cbegin(size_type n) const noexcept { return ht_.cbegin(n); }

  local_iterator end(size_type n) noexcept { return ht_.end(n); }
  const_local_iterator end(size_type n) const noexcept { return ht_.end(n); }
  const_local_iterator cend(size_type n) const noexcept { return ht_.cend(n); }

  size_type bucket_count() const noexcept { return ht_.bucket_count(); }
  size_type max_bucket_count() const noexcept { return ht_.max_bucket_count(); }

  size_type bucket_size(size_type n) const noexcept { return ht_.bucket_size(n); }
  size_type bucket(const key_type& key) const { return ht_.bucket(key); }

  // hash policy
  float load_factor() const noexcept { return ht_.load_factor(); }

  float max_load_factor() const noexcept { return ht_.max_load_factor(); }
  void max_load_factor(float ml) { ht_.max_load_factor(ml); }

  void rehash(size_type count) { ht_.rehash(count); }
  void reserve(size_type count) { ht_.reserve(count); }

  hasher hash_fcn() const { return ht_.hash_fcn(); }
  key_equal key_eq() const { return ht_.key_eq(); }

 public:
  friend bool operator==(const UnorderedMultiSet& lhs, const UnorderedMultiSet& rhs) {
    return lhs.ht_.equal_to_multi(rhs.ht_);
  }
  friend bool operator!=(const UnorderedMultiSet& lhs, const UnorderedMultiSet& rhs) {
    return !lhs.ht_.equal_to_multi(rhs.ht_);
  }
};

template <typename Key, typename Hash, typename KeyEqual>
bool operator==(
    const UnorderedMultiSet<Key, Hash, KeyEqual>& lhs,
    const UnorderedMultiSet<Key, Hash, KeyEqual>& rhs) {
  return lhs == rhs;
}

template <typename Key, typename Hash, typename KeyEqual>
bool operator!=(
    const UnorderedMultiSet<Key, Hash, KeyEqual>& lhs,
    const UnorderedMultiSet<Key, Hash, KeyEqual>& rhs) {
  return lhs != rhs;
}

template <typename Key, typename Hash, typename KeyEqual>
void swap(
    const UnorderedMultiSet<Key, Hash, KeyEqual>& lhs,
    const UnorderedMultiSet<Key, Hash, KeyEqual>& rhs) {
  lhs.swap(rhs);
}

}  // namespace mystl
#endif  // !MYTINYSTL_UNORDERED_SET_H_
