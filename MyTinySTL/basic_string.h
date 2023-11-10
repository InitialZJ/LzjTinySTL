#ifndef MYTINYSTL_BASIC_STRING_H_
#define MYTINYSTL_BASIC_STRING_H_

// 包含一个模板类basic_string
// 用于表示字符串类型

#include <iostream>

#include "exceptdef.h"
#include "functional.h"
#include "iterator.h"
#include "memory.h"

namespace mystl {

// char_traits

template <typename CharType>
struct CharTraits {
  using char_type = CharType;

  static size_t length(const char_type* str) {
    size_t len = 0;
    for (; *str != char_type(0); ++str) {
      ++len;
    }
    return len;
  }

  static int compare(const char_type* s1, const char_type* s2, size_t n) {
    for (; n != 0; --n, ++s1, ++s2) {
      if (*s1 < *s2) {
        return -1;
      }
      if (*s2 < *s1) {
        return 1;
      }
    }
    return 0;
  }

  static char_type* copy(char_type* dst, const char_type* src, size_t n) {
    MYSTL_DEBUG(src + n <= dst || dst + n <= src);
    char_type* r = dst;
    for (; n != 0; --n, ++dst, ++src) {
      *dst = *src;
    }
    return r;
  }

  static char_type* move(char_type* dst, const char_type* src, size_t n) {
    char_type* r = dst;
    if (dst < src) {
      for (; n != 0; --n, ++dst, ++src) {
        *dst = *src;
      }
    } else if (src < dst) {
      dst += n;
      src += n;
      for (; n != 0; --n) {
        *--dst = *--src;
      }
    }
    return r;
  }

  static char_type* fill(char_type* dst, char_type ch, size_t count) {
    char_type* r = dst;
    for (; count > 0; --count, ++dst) {
      *dst = ch;
    }
    return r;
  }
};

// 偏特化
template <>
struct CharTraits<char> {
  using char_type = char;

  static size_t length(const char_type* str) noexcept {
    return std::strlen(str);
  }

  static int compare(const char_type* s1, const char_type* s2,
                     size_t n) noexcept {
    return std::memcmp(s1, s2, n);
  }

  static char_type* copy(char_type* dst, const char_type* src,
                         size_t n) noexcept {
    MYSTL_DEBUG(src + n <= dst || dst + n <= src);
    return static_cast<char_type*>(std::memcpy(dst, src, n));
  }

  static char_type* move(char_type* dst, const char_type* src,
                         size_t n) noexcept {
    return static_cast<char_type*>(std::memmove(dst, src, n));
  }

  static char_type* fill(char_type* dst, char_type ch, size_t count) noexcept {
    return static_cast<char_type*>(std::memset(dst, ch, count));
  }
};

// 偏特化
template <>
struct CharTraits<wchar_t> {
  using char_type = wchar_t;

  static size_t length(const char_type* str) noexcept {
    return std::wcslen(str);
  }

  static int compare(const char_type* s1, const char_type* s2,
                     size_t n) noexcept {
    return std::wmemcmp(s1, s2, n);
  }

  static char_type* copy(char_type* dst, const char_type* src,
                         size_t n) noexcept {
    MYSTL_DEBUG(src + n <= dst || dst + n <= src);
    return static_cast<char_type*>(std::wmemcpy(dst, src, n));
  }

  static char_type* move(char_type* dst, const char_type* src,
                         size_t n) noexcept {
    return static_cast<char_type*>(std::wmemmove(dst, src, n));
  }

  static char_type* fill(char_type* dst, char_type ch, size_t count) noexcept {
    return static_cast<char_type*>(std::wmemset(dst, ch, count));
  }
};

// 偏特化
template <>
struct CharTraits<char16_t> {
  using char_type = char16_t;

  static size_t length(const char_type* str) noexcept {
    size_t len = 0;
    for (; *str != char_type(0); ++str) {
      ++len;
    }
    return len;
  }

  static int compare(const char_type* s1, const char_type* s2,
                     size_t n) noexcept {
    for (; n != 0; --n, ++s1, ++s2) {
      if (*s1 < *s2) {
        return -1;
      }
      if (*s2 < *s1) {
        return 1;
      }
    }
    return 0;
  }

  static char_type* copy(char_type* dst, const char_type* src,
                         size_t n) noexcept {
    MYSTL_DEBUG(src + n <= dst || dst + n <= src);
    char_type* r = dst;
    for (; n != 0; --n, ++dst, ++src) {
      *dst = *src;
    }
    return r;
  }

  static char_type* move(char_type* dst, const char_type* src,
                         size_t n) noexcept {
    char_type* r = dst;
    if (dst < src) {
      for (; n != 0; --n, ++dst, ++src) {
        *dst = *src;
      }
    } else if (src < dst) {
      dst += n;
      src += n;
      for (; n != 0; --n) {
        *--dst = *--src;
      }
    }
    return r;
  }

  static char_type* fill(char_type* dst, char_type ch, size_t count) {
    char_type* r = dst;
    for (; count > 0; --count, ++dst) {
      *dst = ch;
    }
    return r;
  }
};

// 偏特化
template <>
struct CharTraits<char32_t> {
  using char_type = char32_t;

  static size_t length(const char_type* str) noexcept {
    size_t len = 0;
    for (; *str != char_type(0); ++str) {
      ++len;
    }
    return len;
  }

  static int compare(const char_type* s1, const char_type* s2,
                     size_t n) noexcept {
    for (; n != 0; --n, ++s1, ++s2) {
      if (*s1 < *s2) {
        return -1;
      }
      if (*s2 < *s1) {
        return 1;
      }
    }
    return 0;
  }

  static char_type* copy(char_type* dst, const char_type* src,
                         size_t n) noexcept {
    MYSTL_DEBUG(src + n <= dst || dst + n <= src);
    char_type* r = dst;
    for (; n != 0; --n, ++dst, ++src) {
      *dst = *src;
    }
    return r;
  }

  static char_type* move(char_type* dst, const char_type* src,
                         size_t n) noexcept {
    char_type* r = dst;
    if (dst < src) {
      for (; n != 0; --n, ++dst, ++src) {
        *dst = *src;
      }
    } else if (src < dst) {
      dst += n;
      src += n;
      for (; n != 0; --n) {
        *--dst = *--src;
      }
    }
    return r;
  }

  static char_type* fill(char_type* dst, char_type ch, size_t count) {
    char_type* r = dst;
    for (; count > 0; --count, ++dst) {
      *dst = ch;
    }
    return r;
  }
};

// 初始化basic_string尝试分配的最小buffer大小，可能被忽略
#define STRING_INIT_SIZE 32

// 模板类basic_string
// 参数一代表字符类型，参数二代表萃取字符类型的方式，缺省使用mystl::char_traits
template <typename CharType, typename CharTraits = mystl::CharTraits<CharType>>
class BasicString {
 public:
  using traits_type = CharTraits;
  using char_traits = CharTraits;

  using allocator_type = mystl::Allocator<CharType>;
  using data_allocator = mystl::Allocator<CharType>;

  using value_type = typename allocator_type::value_type;
  using pointer = typename allocator_type::pointer;
  using const_pointer = typename allocator_type::const_pointer;
  using reference = typename allocator_type::reference;
  using const_reference = typename allocator_type::const_reference;
  using size_type = typename allocator_type::size_type;
  using difference_type = typename allocator_type::difference_type;

  using iterator = value_type*;
  using const_iterator = const value_type*;
  using reverse_iterator = mystl::ReverseIterator<iterator>;
  using const_reverse_iterator = mystl::ReverseIterator<const_iterator>;

  allocator_type get_allocator() { return allocator_type(); }

  static_assert(std::is_pod<CharType>::value,
                "Charater type of basic_string must be a POD");
  static_assert(std::is_same<CharType, typename traits_type::char_type>::value,
                "CharType must be same as traits_type::char_type");

 public:
  // 末尾位置的值，例：
  // if (str.find('a') != string::npos) { /* do something */ }
  static constexpr size_type npos = static_cast<size_type>(-1);

 private:
  iterator buffer_;  // 储存字符串的起始位置
  size_type size_;   // 大小
  size_type cap_;    // 容量

 public:
  // 构造、复制、移动、析构函数
  BasicString() noexcept { try_init(); }

  BasicString(size_type n, value_type ch)
      : buffer_(nullptr), size_(0), cap_(0) {
    fill_init(n, ch);
  }

  BasicString(const BasicString& other, size_type pos)
      : buffer_(nullptr), size_(0), cap_(0) {
    init_from(other.buffer_, pos, other.size_ - pos);
  }

  BasicString(const BasicString& other, size_type pos, size_type count)
      : buffer_(nullptr), size_(0), cap_(0) {
    init_from(other.buffer_, pos, count);
  }

  BasicString(const_pointer str) : buffer_(nullptr), size_(0), cap_(0) {
    init_from(str, 0, char_traits::length(str));
  }

  BasicString(const_pointer str, size_type count)
      : buffer_(nullptr), size_(0), cap_(0) {
    init_from(str, 0, count);
  }

  template <typename Iter,
            typename std::enable_if<mystl::IsInputIterator<Iter>::value,
                                    int>::type = 0>
  BasicString(Iter first, Iter last) {
    copy_init(first, last, iterator_category(first));
  }

  BasicString(const BasicString& rhs) : buffer_(nullptr), size_(0), cap_(0) {
    init_from(rhs.buffer_, 0, rhs.size_);
  }

  BasicString(BasicString&& rhs) noexcept
      : buffer_(rhs.buffer_), size_(rhs.size_), cap_(rhs.cap_) {
    rhs.buffer_ = nullptr;
    rhs.size_ = 0;
    rhs.cap_ = 0;
  }

  BasicString& operator=(const BasicString& rhs);
  BasicString& operator=(BasicString&& rhs) noexcept;

  BasicString& operator=(const_pointer str);
  BasicString& operator=(value_type ch);

  ~BasicString() { destroy_buffer(); }

 public:
  // 迭代器相关操作
  iterator begin() noexcept { return buffer_; }
  const_iterator begin() const noexcept { return buffer_; }
  iterator end() noexcept { return buffer_ + size_; }
  const_iterator end() const noexcept { return buffer_ + size_; }

  reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator(end());
  }
  reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator(begin());
  }

  const_iterator cbegin() const noexcept { return begin(); }
  const_iterator cend() const noexcept { return end(); }
  const_reverse_iterator crbegin() const noexcept { return rbegin(); }
  const_reverse_iterator crend() const noexcept { return rend(); }

  // 容量相关操作
  bool empty() const noexcept { return size_ == 0; }

  size_type size() const noexcept { return size_; }
  size_type length() const noexcept { return size_; }
  size_type capacity() const noexcept { return cap_; }
  size_type max_size() const noexcept { return static_cast<size_type>(-1); }

  void reserve(size_type n);
  void shrink_to_fit();

  // 访问元素相关操作
  reference operator[](size_type n) {
    MYSTL_DEBUG(n <= size_);
    if (n == size_) {
      *(buffer_ + n) = value_type();
    }
    return *(buffer_ + n);
  }
  const_reference operator[](size_type n) const {
    MYSTL_DEBUG(n <= size_);
    if (n == size_) {
      *(buffer_ + n) = value_type();
    }
    return *(buffer_ + n);
  }

  reference at(size_type n) {
    THROW_OUT_OF_RANGE_IF(
        n >= size_, "basic_string<Char, Traits>::at() subscript out of range");
    return (*this)[n];
  }
  const_reference at(size_type n) const {
    THROW_OUT_OF_RANGE_IF(
        n >= size_, "basic_string<Char, Traits>::at() subscript out of range");
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

  const_pointer data() const noexcept { return to_raw_pointer(); }
  const_pointer c_str() const noexcept { return to_raw_pointer(); }

  // insert
  iterator insert(const_iterator pos, value_type ch);
  iterator insert(const_iterator pos, size_type count, value_type ch);

  template <typename Iter>
  iterator insert(const_iterator pos, Iter first, Iter last);

  // push_back / pop_back
  void push_back(value_type ch) { append(1, ch); }
  void pop_back() {
    MYSTL_DEBUG(!empty());
    --size_;
  }

  // append
  BasicString& append(size_type count, value_type ch);

  BasicString& append(const BasicString& str) {
    return append(str, 0, str.size_);
  }
  BasicString& append(const BasicString& str, size_type pos) {
    return append(str, pos, str.size_ - pos);
  }
  BasicString& append(const BasicString& str, size_type pos, size_type count);

  BasicString& append(const_pointer s) {
    return append(s, CharTraits::lenght(s));
  }
  BasicString& append(const_pointer s, size_type count);

  template <typename Iter,
            typename std::enable_if<mystl::IsInputIterator<Iter>::kValue,
                                    int>::type = 0>
  BasicString& append(Iter first, Iter last) {
    return append_range(first, last);
  }

  // erase / clear
  iterator erase(const_iterator pos);
  iterator erase(const_iterator first, const_iterator last);

  // resize
  void resize(size_type count) { resize(count, value_type()); }
  void resize(size_type count, value_type ch);

  void clear() noexcept { size_ = 0; }

  // compare
  int compare(const BasicString& other) const;
  int compare(size_type pos1, size_type count1, const BasicString& other) const;
  int compare(size_type pos1, size_type count1, const BasicString& other,
              size_type pos2, size_type count2 = npos) const;
  int compare(const_pointer s) const;
  int compare(size_type pos1, size_type count1, const_pointer s) const;
  int compare(size_type pos1, size_type count1, const_pointer s,
              size_type count2) const;

  // substr
  BasicString substr(size_type index, size_type count = npos) {
    count = mystl::min(count, size_ - index);
    return BasicString(buffer_ + index, buffer_ + index + count);
  }

  // replace
  BasicString& replace(size_type pos, size_type count, const BasicString& str) {
    THROW_OUT_OF_RANGE_IF(
        pos > size_, "basic_string<Char, Traits>::replace's pos out of range");
    return replace_cstr(buffer_ + pos, count, str.buffer_, str.size_);
  }
  BasicString& replace(const_iterator first, const_iterator last,
                       const BasicString& str) {
    MYSTL_DEBUG(begin() <= first && last <= end() && first <= last);
    return replace_cstr(first, static_cast<size_type>(last - first),
                        str.buffer_, str.size_);
  }

  BasicString& replace(size_type pos, size_type count, const_pointer str) {
    THROW_OUT_OF_RANGE_IF(
        pos > size_, "basic_string<Char, Traits>::replace's pos out of range");
    return replace_cstr(buffer_ + pos, count, str, CharTraits::length(str));
  }
  BasicString& replace(const_iterator first, const_iterator last,
                       const_pointer str) {
    MYSTL_DEBUG(begin() <= first && last <= end() && first <= last);
    return replace_cstr(first, static_cast<size_type>(last - first), str,
                        CharTraits::length(str));
  }

  BasicString& replace(size_type pos, size_type count, const_pointer str,
                       size_type count2) {
    THROW_OUT_OF_RANGE_IF(
        pos > size_, "basic_string<Char, Traits>::replace's pos out of range");
    return replace_cstr(buffer_ + pos, count, str, count2);
  }
  BasicString& replace(const_iterator first, const_iterator last,
                       const_pointer str, size_type count) {
    MYSTL_DEBUG(begin() <= first && last <= end() && first <= last);
    return replace_cstr(first, static_cast<size_type>(last - first), str,
                        count);
  }

  BasicString& replace(size_type pos, size_type count, size_type count2,
                       value_type ch) {
    THROW_OUT_OF_RANGE_IF(
        pos > size_, "basic_string<Char, Traits>::replace's pos out of range");
    return replace_fill(buffer_ + pos, count, count2, ch);
  }
  BasicString& replace(const_iterator first, const_iterator last,
                       size_type count, value_type ch) {
    MYSTL_DEBUG(begin() <= first && last <= end() && first <= last);
    return replace_fill(first, static_cast<size_type>(last - first), count, ch);
  }

  BasicString& replace(size_type pos1, size_type count1, const BasicString& str,
                       size_type pos2, size_type count2 = npos) {
    THROW_OUT_OF_RANGE_IF(
        pos1 > size_ || pos2 > str.size_,
        "basic_string<Char, Traits>::replace's pos out of range");
    return replace_fill(buffer_ + pos1, count1, str.buffer_ + pos2, count2);
  }

  template <typename Iter,
            typename std::enable_if<mystl::IsInputIterator<Iter>::kValue,
                                    int>::type = 0>
  BasicString& replace(const_iterator first, const_iterator last, Iter first2,
                       Iter last2) {
    MYSTL_DEBUG(begin() <= first && last <= end() && first <= last);
    return replace_copy(first, last, first2, last2);
  }

  // reverse
  void reverse() noexcept;

  // swap
  void swap(BasicString& rhs) noexcept;

  // 查找相关操作
  size_type find(value_type ch, size_type pos = 0) const noexcept;
  size_type find(const_pointer str, size_type pos = 0) const noexcept;
  size_type find(const_pointer str, size_type pos,
                 size_type count) const noexcept;
  size_type find(const BasicString& str, size_type pos = 0) const noexcept;

  size_type rfind(value_type ch, size_type pos = npos) const noexcept;
  size_type rfind(const_pointer str, size_type pos = npos) const noexcept;
  size_type rfind(const_pointer str, size_type pos,
                  size_type count) const noexcept;
  size_type rfind(const BasicString& str, size_type pos = npos) const noexcept;

  size_type find_first_of(value_type ch, size_type pos = 0) const noexcept;
  size_type find_first_of(const_pointer str, size_type pos = 0) const noexcept;
  size_type find_first_of(const_pointer str, size_type pos,
                          size_type count) const noexcept;
  size_type find_first_of(const BasicString& str,
                          size_type pos = 0) const noexcept;

  size_type find_first_not_of(value_type ch, size_type pos = 0) const noexcept;
  size_type find_first_not_of(const_pointer str,
                              size_type pos = 0) const noexcept;
  size_type find_first_not_of(const_pointer str, size_type pos,
                              size_type count) const noexcept;
  size_type find_first_not_of(const BasicString& str,
                              size_type pos = 0) const noexcept;

  size_type find_last_of(value_type ch, size_type pos = 0) const noexcept;
  size_type find_last_of(const_pointer str, size_type pos = 0) const noexcept;
  size_type find_last_of(const_pointer str, size_type pos,
                         size_type count) const noexcept;
  size_type find_last_of(const BasicString& str,
                         size_type pos = 0) const noexcept;

  size_type find_last_not_of(value_type ch, size_type pos = 0) const noexcept;
  size_type find_last_not_of(const_pointer str,
                             size_type pos = 0) const noexcept;
  size_type find_last_not_of(const_pointer str, size_type pos,
                             size_type count) const noexcept;
  size_type find_last_not_of(const BasicString& str,
                             size_type pos = 0) const noexcept;

  // count
  size_type count(value_type ch, size_type pos = 0) const noexcept;

 public:
  // 重载operator+=
  BasicString& operator+=(const BasicString& str) { return append(str); }
  BasicString& operator+=(value_type str) { return append(1, str); }
  BasicString& operator+=(const_pointer str) {
    return append(str, str + CharTraits::length(str));
  }

  // 重载operator >> / operator <<
  friend std::istream& operator>>(std::istream& is, BasicString& str) {
    value_type* buf = new value_type[4096];
    is >> buf;
    BasicString tmp(buf);
    str = std::move(tmp);
    delete[] buf;
    return is;
  }

  friend std::ostream& operator>>(std::ostream& os, const BasicString& str) {
    for (size_type i = 0; i < str.size_; ++i) {
      os << *(str.buffer_ + i);
    }
    return os;
  }

 private:
  // helper functions

  // init / destroy
  void try_init() noexcept;

  void fill_init(size_type n, value_type ch);

  template <typename Iter>
  void copy_init(Iter first, Iter last, mystl::InputIteratorTag);
  template <typename Iter>
  void copy_init(Iter first, Iter last, mystl::ForwardIteratorTag);

  void init_from(const_pointer src, size_type pos, size_type n);

  void destroy_buffer();

  // get raw pointer
  const_pointer to_raw_pointer() const;

  // shrink to fit
  void reinsert(size_type size);

  // append
  template <typename Iter>
  BasicString& append_range(Iter first, Iter last);

  // compare
  int compare_cstr(const_pointer s1, size_type n1, const_pointer s2,
                   size_type n2) const;

  // replace
  BasicString& replace_cstr(const_iterator first, size_type count1,
                            const_pointer str, size_type count2);
  BasicString& replace_fill(const_iterator first, size_type count1,
                            size_type count2, value_type ch);
  template <typename Iter>
  BasicString& replace_copy(const_iterator first, const_iterator last,
                            Iter first2, Iter last2);

  // reallocate
  void reallocate(size_type need);
  iterator reallocate_and_fill(iterator pos, size_type n, value_type ch);
  iterator reallocate_and_copy(iterator pos, const_iterator first,
                               const_iterator last);
};

// 复制赋值操作符
template <typename CharType, typename CharTraits>
BasicString<CharType, CharTraits>& BasicString<CharType, CharTraits>::operator=(
    const BasicString& rhs) {
  if (this != &rhs) {
    BasicString tmp(rhs);
    swap(tmp);
  }
  return *this;
}

// 移动赋值操作符
template <typename CharType, typename CharTraits>
BasicString<CharType, CharTraits>& BasicString<CharType, CharTraits>::operator=(
    BasicString&& rhs) noexcept {
  destroy_buffer();
  buffer_ = rhs.buffer_;
  size_ = rhs.size_;
  cap_ = rhs.cap_;
  rhs.buffer_ = nullptr;
  rhs.size_ = 0;
  rhs.cap_ = 0;
  return *this;
}

// 用一个字符串赋值
template <typename CharType, typename CharTraits>
BasicString<CharType, CharTraits>& BasicString<CharType, CharTraits>::operator=(
    const_pointer str) {
  const size_type len = CharTraits::length(str);
  if (cap_ < len) {
    auto new_buffer = data_allocator::allocate(len + 1);
    data_allocator::deallocate(buffer_);
    buffer_ = new_buffer;
    cap_ = len + 1;
  }
  CharTraits::copy(buffer_, str, len);
  size_ = len;
  return *this;
}

// 用一个字符赋值
template <typename CharType, typename CharTraits>
BasicString<CharType, CharTraits>& BasicString<CharType, CharTraits>::operator=(
    value_type ch) {
  if (cap_ < 1) {
    auto new_buffer = data_allocator::allocate(2);
    data_allocator::deallocate(buffer_);
    buffer_ = new_buffer;
    cap_ = 2;
  }
  *buffer_ = ch;
  size_ = 1;
  return *this;
}

// 预留储存空间
template <typename CharType, typename CharTraits>
void BasicString<CharType, CharTraits>::reserve(size_type n) {
  if (cap_ < n) {
    THROW_LENGTH_ERROR_IF(n > max_size(),
                          "n can not lager than max_size() in "
                          "BasicString<Char, Traits>::reserve(n)");
    auto new_buffer = data_allocator::allocate(n);
    char_traits::move(new_buffer, buffer_, size_);
    buffer_ = new_buffer;
    cap_ = n;
  }
}

// 减少不用的空间
template <typename CharType, typename CharTraits>
void BasicString<CharType, CharTraits>::shrink_to_fit() {
  if (size_ != cap_) {
    reinsert(size_);
  }
}

// 在pos处插入一个元素
template <typename CharType, typename CharTraits>
typename BasicString<CharType, CharTraits>::iterator
BasicString<CharType, CharTraits>::insert(const_iterator pos, value_type ch) {
  iterator r = const_cast<iterator>(pos);
  if (size_ == cap_) {
    return reallocate_and_fill(r, 1, ch);
  }
  char_traits::move(r + 1, r, end() - r);
  ++size_;
  *r = ch;
  return r;
}

// 在pos处插入n个元素
template <typename CharType, typename CharTraits>
typename BasicString<CharType, CharTraits>::iterator
BasicString<CharType, CharTraits>::insert(const_iterator pos, size_type count,
                                          value_type ch) {
  iterator r = const_cast<iterator>(pos);
  if (count == 0) {
    return r;
  }
  if (cap_ - size_ < count) {
    return reallocate_and_fill(r, count, ch);
  }
  if (pos == end()) {
    char_traits::fill(end(), ch, count);
    size_ += count;
    return r;
  }
  // TODO: 原版代码有问题，以下是修改代码
  // char_traits::move(r + count, r, count);  // 原代码
  char_traits::move(r + count, r, end() - pos);
  char_traits::fill(r, ch, count);
  size_ += count;
  return r;
}

// 在pos处插入[first, last)内的元素
template <typename CharType, typename CharTraits>
template <typename Iter>
typename BasicString<CharType, CharTraits>::iterator
BasicString<CharType, CharTraits>::insert(const_iterator pos, Iter first,
                                          Iter last) {
  iterator r = const_cast<iterator>(pos);
  const size_type len = mystl::distance(first, last);
  if (len == 0) {
    return r;
  }
  if (cap_ - size_ < len) {
    return reallocate_and_copy(r, first, last);
  }
  if (pos == end()) {
    mystl::uninitialized_copy(first, last, end());
    size_ += len;
    return r;
  }
  // char_traits::move(r + len, r, len);  // TODO: 有问题
  char_traits::move(r + len, r, end() - pos);
  mystl::uninitialized_copy(first, last, r);
  size_ += len;
  return r;
}

// 在末尾添加count个ch
template <typename CharType, typename CharTraits>
BasicString<CharType, CharTraits>& BasicString<CharType, CharTraits>::append(
    size_type count, value_type ch) {
  THROW_LENGTH_ERROR_IF(size_ > max_size() - count,
                        "BasicString<Char, Traits>'s size too big");
  if (cap_ - size_ < count) {
    reallocate(count);
  }
  char_traits::fill(buffer_ + size_, ch, count);
  size_ += count;
  return *this;
}

// 在末尾添加[str[pos] str[pos + count])一段
template <typename CharType, typename CharTraits>
BasicString<CharType, CharTraits>& BasicString<CharType, CharTraits>::append(
    const BasicString& str, size_type pos, size_type count) {
  THROW_LENGTH_ERROR_IF(size_ > max_size() - count,
                        "BasicString<Char, Traits>'s size too big");
  if (count == 0) {
    return *this;
  }
  if (cap_ - size_ < count) {
    reallocate(count);
  }
  char_traits::copy(buffer_ + size_, str.buffer_ + pos, count);
  size_ += count;
  return *this;
}

// 在末尾添加[s, s + count)一段
template <typename CharType, typename CharTraits>
BasicString<CharType, CharTraits>& BasicString<CharType, CharTraits>::append(
    const_pointer s, size_type count) {
  THROW_LENGTH_ERROR_IF(size_ > max_size() - count,
                        "BasicString<Char, Traits>'s size too big");
  if (cap_ - size_ < count) {
    reallocate(count);
  }
  char_traits::copy(buffer_ + size_, s, count);
  size_ += count;
  return *this;
}

// 删除pos处的元素
template <typename CharType, typename CharTraits>
typename BasicString<CharType, CharTraits>::iterator
BasicString<CharType, CharTraits>::erase(const_iterator pos) {
  MYSTL_DEBUG(pos != end());
  iterator r = const_cast<iterator>(pos);
  char_traits::move(r, pos + 1, end() - pos - 1);
  --size_;
  return r;
}

// 删除[first, last)的元素
template <typename CharType, typename CharTraits>
typename BasicString<CharType, CharTraits>::iterator
BasicString<CharType, CharTraits>::erase(const_iterator first,
                                         const_iterator last) {
  if (first == begin() && last == end()) {
    clear();
    return end();
  }
  const size_type n = end() - last;
  iterator r = const_cast<iterator>(first);
  char_traits::move(r, last, n);
  size_ -= (last - first);
  return r;
}

// 重置容器大小
template <typename CharType, typename CharTraits>
void BasicString<CharType, CharTraits>::resize(size_type count, value_type ch) {
  if (count < size_) {
    erase(buffer_ + count, buffer_ + size_);
  } else {
    append(count - size_, ch);
  }
}

// 比较两个BasicString，小于返回-1，大于返回1，等于返回0
template <typename CharType, typename CharTraits>
int BasicString<CharType, CharTraits>::compare(const BasicString& other) const {
  return compare_cstr(buffer_, size_, other.buffer_, other.size_);
}

// 从pos1下标开始的count1个字符跟另一个BasicString比较
template <typename CharType, typename CharTraits>
int BasicString<CharType, CharTraits>::compare(size_type pos1, size_type count1,
                                               const BasicString& other) const {
  auto n1 = mystl::min(count1, size_ - pos1);
  return compare_cstr(buffer_ + pos1, n1, other.buffer_, other.size_);
}

// 从pos1下标开始的count1个字符跟另一个BasicString下标pos2开始的count2个字符比较
template <typename CharType, typename CharTraits>
int BasicString<CharType, CharTraits>::compare(size_type pos1, size_type count1,
                                               const BasicString& other,
                                               size_type pos2,
                                               size_type count2) const {
  auto n1 = mystl::min(count1, size_ - pos1);
  auto n2 = mystl::min(count2, other.size_ - pos2);
  return compare_cstr(buffer_, n1, other.buffer_, n2);
}

// 根另一个字符串比较
template <typename CharType, typename CharTraits>
int BasicString<CharType, CharTraits>::compare(const_pointer s) const {
  auto n2 = char_traits::length(s);
  return compare_cstr(buffer_, size_, s, n2);
}

// 从下标pos1开始的count1个字符跟另一个字符串比较
template <typename CharType, typename CharTraits>
int BasicString<CharType, CharTraits>::compare(size_type pos1, size_type count1,
                                               const_pointer s) const {
  auto n1 = mystl::min(count1, size_ - pos1);
  auto n2 = char_traits::length(s);
  return compare_cstr(buffer_, n1, s, n2);
}

// 从下标pos1开始的count1个字符跟另一个字符串的前count2个字符比较
template <typename CharType, typename CharTraits>
int BasicString<CharType, CharTraits>::compare(size_type pos1, size_type count1,
                                               const_pointer s,
                                               size_type count2) const {
  auto n1 = mystl::min(count1, size_ - pos1);
  return compare_cstr(buffer_, n1, s, count2);
}

// 反转BasicString
template <typename CharType, typename CharTraits>
void BasicString<CharType, CharTraits>::reverse() noexcept {
  for (auto i = begin(), j = end(); i < j;) {
    mystl::iter_swap(i++, --j);
  }
}

// 交换两个BasicString
template <typename CharType, typename CharTraits>
void BasicString<CharType, CharTraits>::swap(BasicString& rhs) noexcept {
  if (this != &rhs) {
    mystl::swap(buffer_, rhs.buffer_);
    mystl::swap(size_, rhs.size_);
    mystl::swap(cap_, rhs.cap_);
  }
}

// 从下标pos开始查找字符为ch的元素，若找到返回其下标，否则返回npos
template <typename CharType, typename CharTraits>
typename BasicString<CharType, CharTraits>::size_type
BasicString<CharType, CharTraits>::find(value_type ch,
                                        size_type pos) const noexcept {
  for (auto i = pos; i < size_; ++i) {
    if (*(buffer_ + i) == ch) {
      return i;
    }
  }
  return npos;
}

// 从下标pos开始查找字符串str，若找到返回起始位置的下标，否则返回npos
template <typename CharType, typename CharTraits>
typename BasicString<CharType, CharTraits>::size_type
BasicString<CharType, CharTraits>::find(const_pointer str,
                                        size_type pos) const noexcept {
  const auto len = char_traits::length(str);
  if (len == 0) {
    return pos;
  }
  if (size_ - pos < len) {
    return npos;
  }
  const auto left = size_ - len;
  for (auto i = pos; i <= left; ++i) {
    if (*(buffer_ + i) == *str) {
      size_type j = 1;
      for (; j < len; ++j) {
        if (*(buffer_ + i + j) != *(str + j)) {
          break;
        }
      }
      if (j == len) {
        return i;
      }
    }
  }
  return npos;
}

// 从下标pos开始查找字符串str的前count个字符，若找到返回起始位置的下标，否则返回npos
template <typename CharType, typename CharTraits>
typename BasicString<CharType, CharTraits>::size_type
BasicString<CharType, CharTraits>::find(const_pointer str, size_type pos,
                                        size_type count) const noexcept {
  if (count == 0) {
    return pos;
  }
  if (size_ - pos < count) {
    return npos;
  }
  const auto left = size_ - count;
  for (auto i = pos; i <= left; ++i) {
    if (*(buffer_ + i) == *str) {
      size_type j = 1;
      for (; j < count; ++j) {
        if (*(buffer_ + i + j) != *(str + j)) {
          break;
        }
      }
      if (j == count) {
        return i;
      }
    }
  }
  return npos;
}

// 从下标pos开始查找字符串str，若找到返回起始位置的下标，否则返回npos
template <typename CharType, typename CharTraits>
typename BasicString<CharType, CharTraits>::size_type
BasicString<CharType, CharTraits>::find(const BasicString& str,
                                        size_type pos) const noexcept {
  const size_type count = str.size_;
  if (count == 0) {
    return pos;
  }
  if (size_ - pos < count) {
    return npos;
  }
  const auto left = size_ - count;
  for (auto i = pos; i <= left; ++i) {
    if (*(buffer_ + i) == str.front()) {
      size_type j = 1;
      for (; j < count; ++j) {
        if (*(buffer_ + i + j) != str[j]) {
          break;
        }
      }
      if (j == count) {
        return i;
      }
    }
  }
  return npos;
}

// 从下标pos开始反向查找值为ch的元素，与find类似
template <typename CharType, typename CharTraits>
typename BasicString<CharType, CharTraits>::size_type
BasicString<CharType, CharTraits>::rfind(value_type ch,
                                         size_type pos) const noexcept {
  if (pos >= size_) {
    pos = size_ - 1;
  }
  for (auto i = pos; i != 0; --i) {
    if (*(buffer_ + i) == ch) {
      return i;
    }
  }
  return front() == ch ? 0 : npos;
}

// 从下标pos开始反向查找字符串str，与find类似
template <typename CharType, typename CharTraits>
typename BasicString<CharType, CharTraits>::size_type
BasicString<CharType, CharTraits>::rfind(const_pointer str,
                                         size_type pos) const noexcept {
  if (pos >= size_) {
    pos = size_ - 1;
  }
  const size_type len = char_traits::length(str);
  switch (len) {
    case 0:
    return pos case 1: {
      for (auto i = pos; i != 0; --i) {
        if (*(buffer_ + i) == *str) {
          return i;
        }
      }
      return front() == *str ? 0 : npos;
    }
    default: {
      // len >= 2
      for (auto i = pos; i >= len - 1; --i) {
        if (*(buffer_ + i) == *(str + len - 1)) {
          size_type j = 1;
          for (; j < len; ++j) {
            if (*(buffer_ + i - j) != *(str + len - j - 1)) {
              break;
            }
          }
          if (j == len) {
            return i - len + 1;
          }
        }
      }
      break;
    }
  }
  return npos;
}

// 从下标pos开始反向查找字符串str前count个字符，与find类似
template <typename CharType, typename CharTraits>
typename BasicString<CharType, CharTraits>::size_type
BasicString<CharType, CharTraits>::rfind(const_pointer str, size_type pos,
                                         size_type count) const noexcept {
  if (count == 0) {
    return pos;
  }
  if (pos >= size_) {
    pos = size_ - 1;
  }
  if (pos < count - 1) {
    return npos;
  }
  for (auto i = pos; i >= count - 1; --i) {
    if (*(buffer_ + i) == *(str + count - 1)) {
      size_type j = 1;
      for (; j < count; ++j) {
        if (*(buffer_ + i - j) != *(str + count - j - 1)) {
          break;
        }
      }
      if (j == count) {
        return i - count + 1;
      }
    }
  }
  return npos;
}

// 从下标pos开始反向查找字符串str，与find类似
template <typename CharType, typename CharTraits>
typename BasicString<CharType, CharTraits>::size_type
BasicString<CharType, CharTraits>::rfind(const BasicString& str,
                                         size_type pos) const noexcept {
  const size_type count = str.size_;
  if (count == 0) {
    return pos;
  }
  if (pos >= size_) {
    pos = size_ - 1;
  }
  if (pos < count - 1) {
    return npos;
  }
  for (auto i = pos; i >= count - 1; --i) {
    if (*(buffer_ + i) == str[count - 1]) {
      size_type j = 1;
      for (; j < count; ++j) {
        if (*(buffer_ + i - j) != str[count - j - 1]) {
          break;
        }
      }
      if (j == count) {
        return i - count + 1;
      }
    }
  }
  return npos;
}

// 从下标pos开始查找ch出现的第一个位置
template <typename CharType, typename CharTraits>
typename BasicString<CharType, CharTraits>::size_type
BasicString<CharType, CharTraits>::find_first_of(value_type ch,
                                                 size_type pos) const noexcept {
  for (auto i = pos; i < size_; ++i) {
    if (*(buffer_ + i) == ch) {
      return i;
    }
  }
  return npos;
}

// 从下标pos开始查找字符串s其中的一个字符出现的第一个位置
template <typename CharType, typename CharTraits>
typename BasicString<CharType, CharTraits>::size_type
BasicString<CharType, CharTraits>::find_first_of(const_pointer s,
                                                 size_type pos) const noexcept {
  const size_type len = char_traits::length(s);
  for (auto i = pos; i < size_; ++i) {
    value_type ch = *(buffer_ + i);
    for (size_type j = 0; j < len; ++j) {
      if (ch == *(s + j)) {
        return i;
      }
    }
  }
  return npos;
}

// 从下标pos开始查找字符串s
template <typename CharType, typename CharTraits>
typename BasicString<CharType, CharTraits>::size_type
BasicString<CharType, CharTraits>::find_first_of(
    const_pointer s, size_type pos, size_type count) const noexcept {
  for (auto i = pos; i < size_; ++i) {
    value_type ch = *(buffer_ + i);
    for (size_type j = 0; j < count; ++j) {
      if (ch == *(s + j)) {
        return i;
      }
    }
  }
  return npos;
}

// 从下标pos开始查找字符串str其中的一个字符出现的第一个位置
template <typename CharType, typename CharTraits>
typename BasicString<CharType, CharTraits>::size_type
BasicString<CharType, CharTraits>::find_first_of(const BasicString& str,
                                                 size_type pos) const noexcept {
  for (auto i = pos; i < size_; ++i) {
    value_type ch = *(buffer_ + i);
    for (size_type j = 0; j < str.size_; ++j) {
      if (ch == str[j]) {
        return i;
      }
    }
  }
  return npos;
}

// 从下标pos开始查找与字符串s其中一个字符不相等的第一个位置
template <typename CharType, typename CharTraits>
typename BasicString<CharType, CharTraits>::size_type
BasicString<CharType, CharTraits>::find_first_not_of(
    const_pointer s, size_type pos) const noexcept {
  const size_type len = char_traits::length(s);
  for (auto i = pos; i < size_; ++i) {
    value_type ch = *(buffer_ + i);
    for (size_type j = 0; j < len; ++j) {
      if (ch != *(s + j)) {
        return i;
      }
    }
  }
  return npos;
}

// 从下标pos开始查找与字符串s前count个字符中不相等的第一个位置
template <typename CharType, typename CharTraits>
typename BasicString<CharType, CharTraits>::size_type
BasicString<CharType, CharTraits>::find_first_not_of(
    const_pointer s, size_type pos, size_type count) const noexcept {
  for (auto i = pos; i < size_; ++i) {
    value_type ch = *(buffer_ + i);
    for (size_type j = 0; j < count; ++j) {
      if (ch != *(s + j)) {
        return i;
      }
    }
  }
  return npos;
}

// 从下标pos开始查找与字符串str的字符中不相等的第一个位置
template <typename CharType, typename CharTraits>
typename BasicString<CharType, CharTraits>::size_type
BasicString<CharType, CharTraits>::find_first_not_of(
    const BasicString& str, size_type pos) const noexcept {
  for (auto i = pos; i < size_; ++i) {
    value_type ch = *(buffer_ + i);
    for (size_type j = 0; j < str.size_; ++j) {
      if (ch != str[j]) {
        return i;
      }
    }
  }
  return npos;
}

// 从下标pos开始查找与ch相等的最后一个位置
template <typename CharType, typename CharTraits>
typename BasicString<CharType, CharTraits>::size_type
BasicString<CharType, CharTraits>::find_last_of(value_type ch,
                                                size_type pos) const noexcept {
  for (auto i = size_ - 1; i >= pos; --i) {
    if (*(buffer_ + i) == ch) {
      return i;
    }
  }
  return npos;
}

// 从下标pos开始查找与字符串s其中一个字符相等的最后一个位置
template <typename CharType, typename CharTraits>
typename BasicString<CharType, CharTraits>::size_type
BasicString<CharType, CharTraits>::find_last_of(const_pointer s,
                                                size_type pos) const noexcept {
  const size_type len = char_traits::length(s);
  for (auto i = size_ - 1; i >= pos; --i) {
    value_type ch = *(buffer_ + i);
    for (size_type j = 0; j < len; ++j) {
      if (ch == *(s + j)) {
        return i;
      }
    }
  }
  return npos;
}

// 从下标pos开始查找与字符串s前count个字符中相等的最后一个位置
template <typename CharType, typename CharTraits>
typename BasicString<CharType, CharTraits>::size_type
BasicString<CharType, CharTraits>::find_last_of(
    const_pointer s, size_type pos, size_type count) const noexcept {
  for (auto i = size_ - 1; i >= pos; --i) {
    value_type ch = *(buffer_ + i);
    for (size_type j = 0; j < count; ++j) {
      if (ch == *(s + j)) {
        return i;
      }
    }
  }
  return npos;
}

// 从下标pos开始查找与字符串str字符中相等的最后一个位置
template <typename CharType, typename CharTraits>
typename BasicString<CharType, CharTraits>::size_type
BasicString<CharType, CharTraits>::find_last_of(const BasicString& str,
                                                size_type pos) const noexcept {
  for (auto i = size_ - 1; i >= pos; --i) {
    value_type ch = *(buffer_ + i);
    for (size_type j = 0; j < str.size_; ++j) {
      if (ch == str[j]) {
        return i;
      }
    }
  }
  return npos;
}

// 从下标pos开始查找与ch字符不相等的最后一个位置
template <typename CharType, typename CharTraits>
typename BasicString<CharType, CharTraits>::size_type
BasicString<CharType, CharTraits>::find_last_not_of(
    value_type ch, size_type pos) const noexcept {
  for (auto i = size_ - 1; i >= pos; --i) {
    if (*(buffer_ + i) != ch) {
      return i;
    }
  }
  return npos;
}

// 从下标pos开始查找与字符串s的字符中不相等的最后一个位置
template <typename CharType, typename CharTraits>
typename BasicString<CharType, CharTraits>::size_type
BasicString<CharType, CharTraits>::find_last_not_of(
    const_pointer s, size_type pos) const noexcept {
  const size_type len = char_traits::length(s);
  for (auto i = size_ - 1; i >= pos; --i) {
    value_type ch = *(buffer_ + i);
    for (size_type j = 0; j < len; ++j) {
      if (ch != *(s + j)) {
        return i;
      }
    }
  }
  return npos;
}

// 从下标pos开始查找与字符串s前count个字符中不相等的最后一个位置
template <typename CharType, typename CharTraits>
typename BasicString<CharType, CharTraits>::size_type
BasicString<CharType, CharTraits>::find_last_not_of(
    const_pointer s, size_type pos, size_type count) const noexcept {
  for (auto i = size_ - 1; i >= pos; --i) {
    value_type ch = *(buffer_ + i);
    for (size_type j = 0; j < count; ++j) {
      if (ch != *(s + j)) {
        return i;
      }
    }
  }
  return npos;
}

// 从下标pos开始查找与字符串str字符中不相等的最后一个位置
template <typename CharType, typename CharTraits>
typename BasicString<CharType, CharTraits>::size_type
BasicString<CharType, CharTraits>::find_last_not_of(
    const BasicString& str, size_type pos) const noexcept {
  for (auto i = size_ - 1; i >= pos; --i) {
    value_type ch = *(buffer_ + i);
    for (size_type j = 0; j < str.size_; ++j) {
      if (ch != str[j]) {
        return i;
      }
    }
  }
  return npos;
}

// 返回从下标pos开始字符为ch的元素出现的此书
template <typename CharType, typename CharTraits>
typename BasicString<CharType, CharTraits>::size_type
BasicString<CharType, CharTraits>::count(value_type ch,
                                         size_type pos) const noexcept {
  size_type n = 0;
  for (auto i = pos; i < size_; ++i) {
    if (*(buffer_ + i) == ch) {
      ++n;
    }
  }
  return n;
}

// helper function

// 尝试初始化一段buffer，若分配失败则忽略，不会抛出异常
template <typename CharType, typename CharTraits>
void BasicString<CharType, CharTraits>::try_init() noexcept {
  try {
    buffer_ =
        data_allocator::allocate(static_cast<size_type>(STRING_INIT_SIZE));
    size_ = 0;
    cap_ = 0;
  } catch (...) {
    buffer_ = nullptr;
    size_ = 0;
    cap_ = 0;
    // no throw
  }
}

// fill_init
template <typename CharType, typename CharTraits>
void BasicString<CharType, CharTraits>::fill_init(size_type n, value_type ch) {
  const auto init_size =
      mystl::max(static_cast<size_type>(STRING_INIT_SIZE), n + 1);
  buffer_ = data_allocator::allocate(init_size);
  char_traits::fill(buffer_, ch, n);
  size_ = n;
  cap_ = init_size;
}

// copy_init函数
template <typename CharType, typename CharTraits>
template <typename Iter>
void BasicString<CharType, CharTraits>::copy_init(Iter first, Iter last,
                                                  mystl::InputIteratorTag) {
  size_type n = mystl::distance(first, last);
  const auto init_size =
      mystl::max(static_cast<size_type>(STRING_INIT_SIZE), n + 1);
  try {
    buffer_ = data_allocator::allocate(init_size);
    size_ = n;
    cap_ = init_size;
  } catch (...) {
    buffer_ = nullptr;
    size_ = 0;
    cap_ = 0;
    throw;
  }
  for (; n > 0; --n, ++first) {
    // TODO: append在哪定义的
    append(*first);
  }
}

template <typename CharType, typename CharTraits>
template <typename Iter>
void BasicString<CharType, CharTraits>::copy_init(Iter first, Iter last,
                                                  mystl::ForwardIteratorTag) {
  const size_type n = mystl::distance(first, last);
  const auto init_size =
      mystl::max(static_cast<size_type>(STRING_INIT_SIZE), n + 1);
  try {
    buffer_ = data_allocator::allocate(init_size);
    size_ = n;
    cap_ = init_size;
    mystl::uninitialized_copy(first, last, buffer_);
  } catch (...) {
    buffer_ = nullptr;
    size_ = 0;
    cap_ = 0;
    throw;
  }
}

// init_from函数
template <typename CharType, typename CharTraits>
void BasicString<CharType, CharTraits>::init_from(const_pointer src,
                                                  size_type pos,
                                                  size_type count) {
  const auto init_size =
      mystl::max(static_cast<size_type>(STRING_INIT_SIZE), count + 1);
  buffer_ = data_allocator::allocate(init_size);
  char_traits::copy(buffer_, src + pos, count);
  size_ = count;
  cap_ = init_size;
}

// destroy_buffer
template <typename CharType, typename CharTraits>
void BasicString<CharType, CharTraits>::destroy_buffer() {
  if (buffer_ != nullptr) {
    data_allocator::deallocate(buffer_, cap_);
    buffer_ = nullptr;
    size_ = 0;
    cap_ = 0;
  }
}

// to_raw_pointer
template <typename CharType, typename CharTraits>
typename BasicString<CharType, CharTraits>::const_pointer
BasicString<CharType, CharTraits>::to_raw_pointer() const {
  *(buffer_ + size_) = value_type();
  return buffer_;
}

// reinsert函数
template <typename CharType, typename CharTraits>
void BasicString<CharType, CharTraits>::reinsert(size_type size) {
  auto new_buffer = data_allocator::allocate(size);
  try {
    char_traits::move(new_buffer, buffer_, size);
  } catch (...) {
    data_allocator::deallocate(new_buffer);
  }
  buffer_ = new_buffer;
  size_ = size;
  cap_ = size;
}

// append_range，末尾追加一段[first, last)内的字符
template <typename CharType, typename CharTraits>
template <typename Iter>
BasicString<CharType, CharTraits>&
BasicString<CharType, CharTraits>::append_range(Iter first, Iter last) {
  const size_type n = mystl::distance(first, last);
  THROW_LENGTH_ERROR_IF(size_ > max_size() - n,
                        "BasicString<CharType, CharTraits>'s size too big");
  if (cap_ - size_ < n) {
    reallocate(n);
  }
  mystl::uninitialized_copy_n(first, n, buffer_ + size_);
  size_ += n;
  return *this;
}

template <typename CharType, typename CharTraits>
int BasicString<CharType, CharTraits>::compare_cstr(const_pointer s1,
                                                    size_type n1,
                                                    const_pointer s2,
                                                    size_type n2) const {
  auto rlen = mystl::min(n1, n2);
  auto res = char_traits::compare(s1, s2, rlen);
  if (res != 0) {
    return res;
  }
  if (n1 < n2) {
    return -1;
  }
  if (n1 > n2) {
    return 1;
  }
  return 0;
}

// 把first开始的count1个字符替换成str开始的count2个字符
template <typename CharType, typename CharTraits>
BasicString<CharType, CharTraits>&
BasicString<CharType, CharTraits>::replace_cstr(const_iterator first,
                                                size_type count1,
                                                const_pointer str,
                                                size_type count2) {
  if (static_cast<size_type>(cend() - first) < count1) {
    count1 = cend() - first;
  }
  if (count1 < count2) {
    const size_type add = count2 - count1;
    THROW_LENGTH_ERROR_IF(size_ > max_size() - add,
                          "BasicString<CharType, CharTraits>'s size too big");
    if (size_ > cap_ - add) {
      reallocate(add);
    }
    pointer r = const_cast<pointer>(first);
    char_traits::move(r + count2, first + count1, end() - (first + count1));
    char_traits::copy(r, str, count2);
    size_ += add;
  } else {
    pointer r = const_cast<pointer>(first);
    char_traits::move(r + count2, first + count1, end() - (first + count1));
    char_traits::copy(r, str, count2);
    size_ -= (count1 - count2);
  }
  return *this;
}

// 把first开始的count1个字符替换成count2个ch字符
template <typename CharType, typename CharTraits>
BasicString<CharType, CharTraits>&
BasicString<CharType, CharTraits>::replace_fill(const_iterator first,
                                                size_type count1,
                                                size_type count2,
                                                value_type ch) {
  if (static_cast<size_type>(cend() - first) < count1) {
    count1 = cend() - first;
  }
  if (count1 < count2) {
    const size_type add = count2 - count1;
    THROW_LENGTH_ERROR_IF(size_ > max_size() - add,
                          "BasicString<CharType, CharTraits>'s size too big");
    if (size_ > cap_ - add) {
      reallocate(add);
    }
    pointer r = const_cast<pointer>(first);
    char_traits::move(r + count2, first + count1, end() - (first + count1));
    char_traits::fill(r, ch, count2);
    size_ += add;
  } else {
    pointer r = const_cast<pointer>(first);
    char_traits::move(r + count2, first + count1, end() - (first + count1));
    char_traits::fill(r, ch, count2);
    size_ -= (count1 - count2);
  }
  return *this;
}

// 把[first, last)的字符替换成[first2, last2)
template <typename CharType, typename CharTraits>
template <typename Iter>
BasicString<CharType, CharTraits>&
BasicString<CharType, CharTraits>::replace_copy(const_iterator first,
                                                const_iterator last,
                                                Iter first2, Iter last2) {
  size_type len1 = last - first;
  size_type len2 = last2 - first2;
  if (len1 < len2) {
    const size_type add = len2 - len1;
    THROW_LENGTH_ERROR_IF(size_ > max_size() - add,
                          "BasicString<CharType, CharTraits>'s size too big");
    if (size_ > cap_ - add) {
      reallocate(add);
    }  
    pointer r = const_cast<pointer>(first);
    char_traits::move(r + len2, first + len1, end() - (first + len1));
    char_traits::copy(r, first2, len2);
    size_ += add;
  } else {
    pointer r = const_cast<pointer>(first);
    char_traits::move(r + len2, first + len1, end() - (first + len1));
    char_traits::copy(r, first2, len2);
    size_ -= (len1 - len2);
  }
  return *this;
}

}  // namespace mystl

#endif  // !MYTINYSTL_BASIC_STRING_H_
