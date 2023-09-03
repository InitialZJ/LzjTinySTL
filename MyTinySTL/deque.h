#ifndef MYTINYSTL_DEQUE_H_
#define MYTINYSTL_DEQUE_H_

// deque：双端队列

// notes:
//
// 异常保证：
// mystl::deque<T> 满足基本异常保证，部分函数无异常保证，并对以下等函数做强异常安全保证：
//   * emplace_front
//   * emplace_back
//   * emplace
//   * push_front
//   * push_back
//   * insert

#include <initializer_list>

#include "exceptdef.h"
#include "iterator.h"
#include "memory.h"
#include "util.h"

namespace mystl {
#ifdef max
#pragma message("#undefing marco max")
#undef max
#endif  // max

#ifdef min
#pragma message("#undefing marco min")
#undef min
#endif  // min

// deque map初始化的大小
#ifndef DEQUE_MAP_INIT_SIZE
#define DEQUE_MAP_INIT_SIZE 8
#endif

template <typename T>
struct DequeBufSize {
  static constexpr size_t kValue = sizeof(T) < 256 ? 4096 / sizeof(T) : 16;
};

// deque迭代器设计
template <typename T, typename Ref, typename Ptr>
struct DequeIterator : public Iterator<RandomAccessIteratorTag, T> {
  using iterator = DequeIterator<T, T&, T*>;
  using const_iterator = DequeIterator<T, const T&, const T*>;
  using self = DequeIterator;

  using value_type = T;
  using pointer = Ptr;
  using reference = Ref;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using value_pointer = T*;
  using map_pointer = T**;

  static const size_type kBufferSize = DequeBufSize<T>::kValue;

  // 迭代器所含成员数据
  value_pointer cur;    // 指向所在缓冲区的当前元素
  value_pointer first;  // 指向所在缓冲区的头部
  value_pointer last;   // 指向所在缓冲区的尾部
  map_pointer node;     // 缓冲区所在结点

  // 构造、复制、移动函数
  DequeIterator() noexcept : cur(nullptr), first(nullptr), last(nullptr), node(nullptr) {}

  DequeIterator(value_pointer v, map_pointer n)
      : cur(v), first(*n), last(*n + kBufferSize), node(n) {}

  DequeIterator(const iterator& rhs)
      : cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {}

  DequeIterator(iterator&& rhs) noexcept
      : cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {
    rhs.cur = nullptr;
    rhs.first = nullptr;
    rhs.last = nullptr;
    rhs.node = nullptr;
  }

  DequeIterator(const const_iterator& rhs)
      : cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {}

  self& operator=(const iterator& rhs) {
    if (this != &rhs) {
      cur = rhs.cur;
      first = rhs.first;
      last = rhs.last;
      node = rhs.node;
    }
    return *this;
  }

  // 转到另一个缓冲区
  void set_node(map_pointer new_node) {
    node = new_node;
    first = *new_node;
    last = first + kBufferSize;
  }

  // 重载运算符
  reference operator*() const { return *cur; }
  pointer operator->() const { return cur; }

  difference_type operator-(const self& x) const {
    return static_cast<difference_type>(kBufferSize) * (node - x.node) + (cur - first) -
           (x.cur - x.first);
  }

  self& operator++() {
    ++cur;
    if (cur == last) {
      set_node(node + 1);
      cur = first;
    }
    return *this;
  }

  self operator++(int) {
    self tmp = *this;
    ++*this;
    return tmp;
  }

  self& operator--() {
    if (cur == first) {
      set_node(node - 1);
      cur = last;
    }
    --cur;
    return *this;
  }

  self operator--(int) {
    self tmp = *this;
    --*this;
    return tmp;
  }

  self& operator+=(difference_type n) {
    const auto offset = n + (cur - first);
    if (offset >= 0 && offset < static_cast<difference_type>(kBufferSize)) {
      // 仍在当前缓冲区
      cur += n;
    } else {
      // 要跳到其他缓冲区
      const auto node_offset = offset > 0
                                   ? offset / static_cast<difference_type>(kBufferSize)
                                   : -static_cast<difference_type>((-offset - 1) / kBufferSize) - 1;
      set_node(node + node_offset);
      cur = first + (offset - node_offset * static_cast<difference_type>(kBufferSize));
    }
    return *this;
  }

  slef operator+(difference_type n) const {
    self tmp = *this;
    return tmp += n;
  }

  self& operator-=(difference_type n) { return *this += -n; }

  self operator-(difference_type n) const {
    self tmp = *this;
    return tmp -= n;
  }

  reference operator[](difference_type n) const { return *(*this + n); }

  // 重载比较操作符
  bool operator==(const self& rhs) const { return cur == rhs.cur; }
  bool operator<(const self& rhs) const {
    return node == rhs.node ? (cur < rhs.cur) : (node < rhs.node);
  }
  bool operator!=(const self& rhs) const { return !(*this == rhs); }
  bool operator>(const self& rhs) const { return rhs < *this; }
  bool operator<=(const self& rhs) const { return !(rhs < *this); }
  bool operator>=(const self& rhs) const { return !(*this < rhs); }
};

// 模板类deque
// 模板参数代表数据类型
template <typename T>
class Deque {
 public:
  using allocator_type = mystl::Allocator<T>;
  using data_allocator = mystl::Allocator<T>;
  using map_allocator = mystl::Allocator<T>;

  using value_type = typename allocator_type::value_type;
  using pointer = typename allocator_type::pointer;
  using const_pointer = typename allocator_type::const_pointer;
  using reference = typename allocator_type::reference;
  using const_reference = typename allocator_type::const_reference;
  using size_type = typename allocator_type::size_type;
  using difference_type = typename allocator_type::difference_type;
  using map_pointer = pointer*;
  using const_map_pointer = const_pointer*;

  using iterator = DequeIterator<T, T&, T*>;
  using const_iterator = DequeIterator<T, const T&, const T*>;
  using reverse_iterator = mystl::ReverseIterator<iterator>;
  using const_reverse_iterator = mystl::ReverseIterator<const_iterator>;

  allocator_type get_allocator() { return allocator_type(); }

  static const size_type kBufferSize = DequeBufSize<T>::kValue;

 private:
  iterator begin_;  // 指向第一个结点
  iterator end_;    // 指向最后一个结点
  map_pointer map_;  // 指向一块map，map中的每个元素都是一个指针，指向一个缓冲区
  size_type map_size_;  // map内指针的数目

 public:
  Deque() { fill_insert(0, value_type()); }

  explicit Deque(size_type n) { fill_insert(n, value_type()); }

  Deque(size_type n, const value_type& value) { fill_insert(n, value); }

  template <
      typename IIter,
      typename std::enable_if<mystl::IsInputIterator<IIter>::kValue, int>::type = 0>
  Deque(IIter first, IIter last) {
    copy_init(first, last, iterator_category(first));
  }

  Deque(std::initializer_list<value_type> ilist) {
    copy_init(ilist.begin(), ilist.end(), mystl::ForwardIteratorTag());
  }

  Deque(const Deque& rhs) { copy_init(rhs.begin(), rhs.end(), mystl::ForwardIteratorTag()); }

  Deque(Deque&& rhs) noexcept
      : begin_(mystl::move(rhs.begin_)),
        end_(mystl::move(rhs.end_)),
        map_(rhs.map_),
        map_size_(rhs.map_size_) {
    rhs.map_ = nullptr;
    rhs.map_size_ = 0;
  }

  Deque& operator=(const Deque& rhs);
  Deque& operator=(Deque&& rhs);

  Deque& operator=(std::initializer_list<value_type> ilist) {
    Deque tmp(ilist);
    swap(tmp);
    return *this;
  }

  ~Deque() {
    if (map_ != nullptr) {
      clear();
      data_allocator::deallocate(*begin_.node, kBufferSize);
      *begin_.node = nullptr;
      map_allocator::deallocate(map_, map_size_);
      map_ = nullptr;
    }
  }

 public:
  iterator begin() noexcept { return begin_; }
  const_iterator begin() const noexcept { return begin_; }
  iterator end() noexcept { return end_; }
  const_iterator end() const noexcept { return end_; }

  reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const noexcept { return reverse_iterator(end()); }
  reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const noexcept { return reverse_iterator(begin()); }

  const_iterator cbegin() const noexcept { return begin(); }
  const_iterator cend() const noexcept { return end(); }
  const_reverse_iterator crbegin() const noexcept { return rbegin(); }
  const_reverse_iterator crend() const noexcept { return rend(); }

  // 容量相关操作
  bool empty() const noexcept { return begin() == end(); }
  size_type size() const noexcept { return end_ - begin_; }
  size_type max_size() const noexcept { return static_cast<size_type>(-1); }
  void resize(size_type new_size) { resize(new_size, value_type()); }
  void resize(size_type new_size, const value_type& value);
  void shrink_to_fit() noexcept;

  // 访问元素相关操作
  reference operator[](size_type n) {
    MYSTL_DEBUG(n < size());
    return begin_[n];
  }

  const_reference operator[](size_type n) const {
    MYSTL_DEBUG(n < size());
    return begin_[n];
  }

  reference at(size_type n) {
    THROW_OUT_OF_RANGE_IF(!(n < size()), "Deque<T>::at() subscript out of range");
    return (*this)[n];
  }

  const_reference at(size_type n) const {
    THROW_OUT_OF_RANGE_IF(!(n < size()), "Deque<T>::at() subscript out of range");
    return (*this)[n];
  }
}

}  // namespace mystl

#endif  // ! MYTINYSTL_DEQUE_H_