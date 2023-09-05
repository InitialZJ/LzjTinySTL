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

  self operator+(difference_type n) const {
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
    return *(end() - 1);
  }

  const_reference back() const {
    MYSTL_DEBUG(!empty());
    return *(end() - 1);
  }

  // 修改容器相关操作

  // assign

  void assign(size_type n, const value_type& value) { fill_assign(n, value); }

  template <
      typename IIter,
      typename std::enable_if<mystl::IsInputIterator<IIter>::kValue, int>::type = 0>
  void assign(IIter first, IIter last) {
    copy_assign(first, last, iterator_category(first));
  }

  void assign(std::initializer_list<value_type> ilist) {
    copy_assign(ilist.begin(), ilist.end(), mystl::ForwardIteratorTag{});
  }

  // emplace_front / emplace_back / emplace
  template <typename... Args>
  void emplace_front(Args&&... args);
  template <typename... Args>
  void emplace_back(Args&&... args);
  template <typename... Args>
  iterator emplace(iterator pos, Args&&... args);

  // push_front / push_back
  void push_front(const value_type& value);
  void push_back(const value_type& value);

  void push_front(value_type&& value) { emplace_front(mystl::move(value)); }
  void push_back(value_type&& value) { emplace_back(mystl::move(value)); }

  // pop_back / pop_front
  void pop_front();
  void pop_back();

  // insert
  iterator insert(iterator position, const value_type& value);
  iterator insert(iterator position, value_type&& value);
  void insert(iterator position, size_type n, const value_type& value);
  template <
      typename IIter,
      typename std::enable_if<mystl::IsInputIterator<IIter>::kValue, int>::type = 0>
  void insert(iterator position, IIter first, IIter last) {
    insert_dispatch(position, first, last, iterator_category(first));
  }

  // erase / clear
  iterator erase(iterator position);
  iterator erase(iterator first, iterator last);
  void clear();

  // swap
  void swap(Deque& rhs) noexcept;

 private:
  // helper functions

  // create node / destroy node
  map_pointer create_map(size_type size);
  void create_buffer(map_pointer nstart, map_pointer nfinish);
  void destroy_buffer(map_pointer nstart, map_pointer nfinish);

  // initialize
  void map_init(size_type nelem);
  void fill_init(size_type n, const value_type& value);
  template <typename IIter>
  void copy_init(IIter, IIter, InputIteratorTag);
  template <typename FIter>
  void copy_init(FIter, FIter, ForwardIteratorTag);

  // assign
  void fill_assign(size_type n, const value_type& value);
  template <typename IIter>
  void copy_assign(IIter first, IIter last, InputIteratorTag);
  template <typename FIter>
  void copy_assign(FIter first, FIter last, ForwardIteratorTag);

  // insert
  template <typename... Args>
  iterator insert_aux(iterator position, Args&&... args);
  void fill_insert(iterator position, size_type n, const value_type& x);
  template <typename FIter>
  void copy_insert(iterator, FIter, FIter, size_type);
  template <typename IIter>
  void insert_dispatch(iterator, IIter, IIter, InputIteratorTag);
  template <typename FIter>
  void insert_dispatch(iterator, FIter, FIter, ForwardIteratorTag);

  // reallocate
  void require_capacity(size_type n, bool front);
  void reallocate_map_at_front(size_type need);
  void reallocate_map_at_back(size_type need);
};

// 复制赋值运算符
template <typename T>
Deque<T>& Deque<T>::operator=(const Deque& rhs) {
  if (this != &rhs) {
    const auto len = size();
    if (len >= rhs.size()) {
      erase(mystl::copy(rhs.begin_, rhs.end_, begin_), end_);
    } else {
      iterator mid = rhs.begin() + static_cast<difference_type>(len);
      mystl::copy(rhs.begin_, mid, begin_);
      insert(end_, mid, rhs.end_);
    }
  }
  return *this;
}

// 移动赋值运算符
template <typename T>
Deque<T>& Deque<T>::operator=(Deque&& rhs) {
  clear();
  begin_ = mystl::move(rhs.begin_);
  end_ = mystl::move(rhs.end_);
  map_ = rhs.map_;
  map_size_ = rhs.map_size_;
  rhs.map_ = nullptr;
  rhs.map_size_ = 0;
  return *this;
}

// 重置容器大小
template <typename T>
void Deque<T>::resize(size_type new_size, const value_type& value) {
  const auto len = size();
  if (new_size < len) {
    erase(begin_ + new_size, end_);
  } else {
    insert(end_, new_size - len, value);
  }
}

// 减小容器容量
template <typename T>
void Deque<T>::shrink_to_fit() noexcept {
  // 至少会留下头部缓冲区
  // TODO(lzj): 顺序没搞懂
  for (auto cur = map_; cur < begin_.node; ++cur) {
    data_allocator::deallocate(*cur, kBufferSize);
    *cur = nullptr;
  }
  for (auto cur = end_.node + 1; cur < map_ + map_size_; ++cur) {
    data_allocator::deallocate(*cur, kBufferSize);
    *cur = nullptr;
  }
}

// 在头部就地构建元素
template <typename T>
template <typename... Args>
void Deque<T>::emplace_front(Args&&... args) {
  if (begin_.cur != begin_.first) {
    data_allocator::construct(begin_.cur - 1, mystl::forward<Args>(args)...);
    --begin_.cur;
  } else {
    require_capacity(1, true);
    try {
      --begin_;
      data_allocator::construct(begin_.cur, mystl::forward<Args>(args)...);
    } catch (...) {
      ++begin_;
      throw;
    }
  }
}

// 在尾部就地构建元素
template <typename T>
template <typename... Args>
void Deque<T>::emplace_back(Args&&... args) {
  if (end_.cur != end_.last - 1) {
    data_allocator::construct(end_.cur, mystl::forward<Args>(args)...);
    ++end_.cur;
  } else {
    require_capacity(1, false);
    data_allocator::construct(end_.cur, mystl::forward<Args>(args)...);
    ++end_;
  }
}

// 在pos位置就地构建元素
template <typename T>
template <typename... Args>
typename Deque<T>::iterator Deque<T>::emplace(iterator pos, Args&&... args) {
  if (pos.cur == begin_.cur) {
    emplace_front(mystl::forward<Args>(args)...);
    return begin_;
  }
  if (pos.cur == end_.cur) {
    emplace_back(mystl::forward<Args>(args)...);
    return end_ - 1;
  }
  return insert_aux(pos, mystl::forward<Args>(args)...);
}

// 在头部插入元素
template <typename T>
void Deque<T>::push_front(const value_type& value) {
  if (begin_.cur != begin_.first) {
    data_allocator::construct(begin_.cur - 1, value);
    --begin_.cur;
  } else {
    require_capacity(1, true);
    try {
      --begin_;
      data_allocator::construct(begin_.cur, value);
    } catch (...) {
      ++begin_;
      throw;
    }
  }
}

// 在尾部插入元素
template <typename T>
void Deque<T>::push_back(const value_type& value) {
  if (end_.cur != end_.last - 1) {
    data_allocator::construct(end_.cur, value);
    ++end_.cur;
  } else {
    require_capacity(1, false);
    data_allocator::construct(end_.cur, value);
    ++end_;
  }
}

// 弹出头部元素
template <typename T>
void Deque<T>::pop_front() {
  MYSTL_DEBUG(!empty());
  if (begin_.cur != begin_.last - 1) {
    data_allocator::destroy(begin_.cur);
    ++begin_.cur;
  } else {
    data_allocator::destroy(begin_.cur);
    ++begin_;
    destroy_buffer(begin_.node - 1, begin_.node - 1);
  }
}

// 弹出尾部元素
template <typename T>
void Deque<T>::pop_back() {
  MYSTL_DEBUG(!empty());
  if (end_.cur != end_.first) {
    --end_.cur;
    data_allocator::destroy(end_.cur);
  } else {
    --end_;
    data_allocator::destroy(end_.cur);
    destroy_buffer(end_.node + 1, end_.node + 1);
  }
}

// 在position处插入元素
template <typename T>
typename Deque<T>::iterator Deque<T>::insert(iterator position, const value_type& value) {
  if (position.cur == begin_.cur) {
    push_front(value);
    return begin_;
  }
  if (position.cur == end_.cur) {
    push_back(value);
    auto tmp = end_;
    --tmp;
    return tmp;
  }
  return insert_aux(position, value);
}

template <typename T>
typename Deque<T>::iterator Deque<T>::insert(iterator position, value_type&& value) {
  if (position.cur == begin_.cur) {
    emplace_front(mystl::move(value));
    return begin_;
  }
  if (position.cur == end_.cur) {
    emplace_back(mystl::move(value));
    auto tmp = end_;
    --tmp;
    return tmp;
  }
  return insert_aux(position, mystl::move(value));
}

// 在position位置插入n个元素
template <typename T>
void Deque<T>::insert(iterator position, size_type n, const value_type& value) {
  if (position.cur == begin_.cur) {
    require_capacity(n, true);
    auto new_begin = begin_ - n;
    mystl::uninitialized_fill_n(new_begin, n, value);
    begin_ = new_begin;
  }
  if (position.cur == end_.cur) {
    require_capacity(n, false);
    auto new_end = end_ + n;
    mystl::uninitialized_fill_n(end_, n, value);
    end_ = new_end;
  }
  return fill_insert(position, n, value);
}

// 删除position处的元素
template <typename T>
typename Deque<T>::iterator Deque<T>::erase(iterator position) {
  auto next = position;
  ++next;
  const size_type elems_before = position - begin_;
  if (elems_before < (size() / 2)) {
    mystl::copy_backward(begin_, position, next);
    pop_front();
  } else {
    mystl::copy(next, end_, position);
    pop_back();
  }
  return begin_ + elems_before;
}

// 删除[first, last)上的元素
template <typename T>
typename Deque<T>::iterator Deque<T>::erase(iterator first, iterator last) {
  if (first == begin_ && last == end_) {
    clear();
    return end_;
  }
  const size_type len = last - first;
  const size_type elems_before = first - begin_;
  if (elems_before < ((size() - len) / 2)) {
    mystl::copy_backward(begin_, first, last);
    auto new_begin = begin_ + len;
    data_allocator::destroy(begin_.cur, new_begin.cur);
    begin_ = new_begin;
  } else {
    mystl::copy(last, end_, first);
    auto new_end = end_ - len;
    data_allocator::destroy(new_end.cur, end_.cur);
    end_ = new_end;
  }
  return begin_ + elems_before;
}

// 清空Deque
template <typename T>
void Deque<T>::clear() {
  // clear会保留头部的缓冲区
  for (map_pointer cur = begin_.node + 1; cur < end_.node; ++cur) {
    data_allocator::destroy(*cur, *cur + kBufferSize);
  }
  if (begin_.node != end_.node) {
    // 有两个及以上的缓冲区
    mystl::destroy(begin_.cur, begin_.last);
    mystl::destroy(end_.first, end_.cur);
  } else {
    mystl::destroy(begin_.cur, end_.cur);
  }
  shrink_to_fit();
  end_ = begin_;
}

// 交换两个Deque
template <typename T>
void Deque<T>::swap(Deque& rhs) noexcept {
  if (this != &rhs) {
    mystl::swap(begin_, rhs.begin_);
    mystl::swap(end_, rhs.end_);
    mystl::swap(map_, rhs.map_);
    mystl::swap(map_size_, rhs.map_size_);
  }
}

// helper function

template <typename T>
typename Deque<T>::map_pointer Deque<T>::create_map(size_type size) {
  map_pointer mp = nullptr;
  mp = map_allocator::allocate(size);
  for (size_type i = 0; i < size; ++i) {
    *(mp + i) = nullptr;
  }
  return mp;
}

// create_buffer函数
template <typename T>
void Deque<T>::create_buffer(map_pointer nstart, map_pointer nfinish) {
  map_pointer cur;
  try {
    for (cur = nstart; cur <= nfinish; ++cur) {
      *cur = data_allocator::allocate(kBufferSize);
    }
  } catch (...) {
    while (cur != nstart) {
      --cur;
      data_allocator::deallocate(*cur, kBufferSize);
      *cur = nullptr;
    }
    throw;
  }
}

}  // namespace mystl

#endif  // ! MYTINYSTL_DEQUE_H_