#ifndef MYTINYSTL_MEMORY_H_
#define MYTINYSTL_MEMORY_H_

// 负责更高级的动态内存管理
// 包含一些基本函数、空间配置器、未初始化的储存空间管理，以及一个模板类auto_ptr

#include <climits>
#include <cstddef>
#include <cstdlib>
#include <type_traits>

#include "algobase.h"
#include "allocator.h"
#include "construct.h"
#include "iterator.h"
#include "uninitialized.h"
#include "util.h"

namespace mystl {

// 获取对象地址
template <typename Tp>
constexpr Tp* address_of(Tp& value) noexcept {
  return &value;
}

// 获取 / 释放 临时缓冲区

template <typename T>
pair<T*, ptrdiff_t> get_buffer_helper(ptrdiff_t len, T* /*type*/) {
  if (len > static_cast<ptrdiff_t>(INT_MAX / sizeof(T))) {
    len = INT_MAX / sizeof(T);
  }
  while (len > 0) {
    T* tmp = static_cast<T*>(malloc(static_cast<size_t>(len) * sizeof(T)));
    if (tmp) {
      return pair<T*, ptrdiff_t>(tmp, len);
    }
    len /= 2;
  }
  return pair<T*, ptrdiff_t>(nullptr, 0);
}

template <typename T>
pair<T*, ptrdiff_t> get_temporary_buffer(ptrdiff_t len) {
  return get_buffer_helper(len, static_cast<T*>(0));
}

template <typename T>
pair<T*, ptrdiff_t> get_temporary_buffer(ptrdiff_t len, T* /*type*/) {
  return get_buffer_helper(len, static_cast<T*>(0));
}

template <typename T>
void release_temporary_buffer(T* ptr) {
  free(ptr);
}

// 类模板：TemporaryBuffer
// 进行临时缓冲区的申请与释放
template <typename ForwardIterator, typename T>
class TemporaryBuffer {
 private:
  ptrdiff_t original_len_;  // 缓冲区申请的大小
  ptrdiff_t len_;           // 缓冲区实际的大小
  T* buffer_;               // 指向缓冲区的指针

 public:
  // 构造 / 析构 函数
  TemporaryBuffer(ForwardIterator first, ForwardIterator last);

  ~TemporaryBuffer() {
    mystl::destroy(buffer_, buffer_ + len_);
    free(buffer_);
  }

 public:
  ptrdiff_t size() const noexcept { return len_; }
  ptrdiff_t requested_size() const noexcept { return original_len_; }
  T* begin() noexcept { return buffer_; }
  T* end() noexcept { return buffer_ + len_; }

 private:
  void allocate_buffer();
  void initialisze_buffer(const T& /*value*/, std::true_type /*type*/) {}
  void initialisze_buffer(const T& value, std::false_type) {
    mystl::uninitialized_fill_n(buffer_, len_, value);
  }

 private:
  TemporaryBuffer(const TemporaryBuffer&);
  void operator=(const TemporaryBuffer&);
};

template <typename ForwardIterator, typename T>
TemporaryBuffer<ForwardIterator, T>::TemporaryBuffer(Iterator first, ForwardIterator last) {
  try {
    len_ = mystl::distance(first, last);
    allocate_buffer();
    if (len_ > 0) {
      initialisze_buffer(*first, std::is_trivially_default_constructible<T>());
    }
  } catch (...) {
    free(buffer_);
    buffer_ = nullptr;
    len_ = 0;
  }
}

template <typename ForwardIterator, typename T>
void TemporaryBuffer<ForwardIterator, T>::allocate_buffer() {
  original_len_ = len_;
  if (len_ > static_cast<ptrdiff_t>(INT_MAX / sizeof(T))) {
    len_ = INT_MAX / sizeof(T);
  }
  while (len_ > 0) {
    buffer_ = static_cast<T*>(malloc(len_ * sizeof(T)));
    if (buffer_) {
      break;
    }
    len_ /= 2;
  }
}

// 模板类：auto_ptr
// 一个具有严格对象所有权的小型智能指针
template <typename T>
class AutoPtr {
 public:
  using elem_type = T;

 private:
  T* m_ptr_;  // 实际指针

 public:
  explicit AutoPtr(T* p = nullptr) : m_ptr_(p) {}
  AutoPtr(AutoPtr& rhs) : m_ptr_(rhs.release()) {}
  template <typename U>
  AutoPtr(AutoPtr<U>& rhs) : m_ptr_(rhs.release()) {}

  AutoPtr& operator=(AutoPtr& rhs) {
    if (this != &rhs) {
      delete m_ptr_;
      m_ptr_ = rhs.release();
    }
    return *this;
  }

  template <typename U>
  AutoPtr& operator=(AutoPtr<U>& rhs) {
    if (this->get() != rhs.get()) {
      delete m_ptr_;
      m_ptr_ = rhs.release();
    }
    return *this;
  }

  ~AutoPtr() { delete m_ptr_; }

 public:
  T& operator*() const { return *m_ptr_; }
  T* operator->() const { return m_ptr_; }

  // 获得指针
  T* get() const { return m_ptr_; }

  // 释放指针
  T* release() {
    T* tmp = m_ptr_;
    m_ptr_ = nullptr;
    return tmp;
  }

  // 重置指针
  void reset(T* p = nullptr) {
    if (m_ptr_ != p) {
      delete m_ptr_;
      m_ptr_ = p;
    }
  }
};

}  // namespace mystl

#endif  // !MYTINYSTL_MEMORY_H_
