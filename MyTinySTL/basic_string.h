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
  BasicString& append(const_iterator s, size_type count);

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
};

}  // namespace mystl

#endif  // !MYTINYSTL_BASIC_STRING_H_
