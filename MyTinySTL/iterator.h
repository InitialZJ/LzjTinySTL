#ifndef MYTINYSTL_ITERATOR_H_
#define MYTINYSTL_ITERATOR_H_

// 用于迭代器设计，包含一些模板结构体和全局函数
// 对应书3.1节

#include <cstddef>

#include "type_traits.h"

namespace mystl {

// 五种迭代器类型
struct InputIteratorTag {};
struct OutputIteratorTag {};
struct ForwardIteratorTag : public InputIteratorTag {};
struct BidirectionalIteratorTag : public ForwardIteratorTag {};
struct RandomAccessIteratorTag : public BidirectionalIteratorTag {};

// iterator模板
template <
    typename Category,
    typename T,
    typename Distance = ptrdiff_t,
    typename Pointer = T*,
    typename Reference = T&>
struct Iterator {
  using iterator_category = Category;
  using value_type = T;
  using pointer = Pointer;
  using reference = Reference;
  using difference_type = Distance;
};

// iterator traits

template <typename T>
struct HasIteratorCat {
 private:
  struct Two {
    char a;
    char b;
  };
  template <typename U>
  static Two test(...);

  template <typename U>
  static char test(typename U::iterator_category* = 0);

 public:
  static const bool kValue = sizeof(test<T>(0)) == sizeof(char);
};

template <typename Iterator, bool>
struct IteratorTraitsImpl {};

template <typename Iterator>
struct IteratorTraitsImpl<Iterator, true> {
  using iterator_category = typename Iterator::iterator_category;
  using value_type = typename Iterator::value_type;
  using pointer = typename Iterator::pointer;
  using reference = typename Iterator::reference;
  using difference_type = typename Iterator::difference_type;
};

template <typename Iterator, bool>
struct IteratorTraitsHelper {};

template <typename Iterator>
struct IteratorTraitsHelper<Iterator, true>
    : public IteratorTraitsImpl<
          Iterator,
          std::is_convertible<typename Iterator::iterator_category, InputIteratorTag>::value ||
              std::is_convertible<typename Iterator::iterator_category, OutputIteratorTag>::value> {
};

// 萃取迭代器的特性
template <typename Iterator>
struct IteratorTraits : public IteratorTraitsHelper<Iterator, HasIteratorCat<Iterator>::kValue> {};

// 针对原生指针的偏特化版本
template <typename T>
struct IteratorTraits<T*> {
  using iterator_category = RandomAccessIteratorTag;
  using value_type = T;
  using pointer = T*;
  using reference = T&;
  using difference_type = ptrdiff_t;
};

template <typename T>
struct IteratorTraits<const T*> {
  using iterator_category = RandomAccessIteratorTag;
  using value_type = T;
  using pointer = const T*;
  using reference = const T&;
  using difference_type = ptrdiff_t;
};

template <typename T, typename U, bool = HasIteratorCat<IteratorTraits<T>>::kValue>
struct HasIteratorCatOf
    : public m_bool_constant<
          std::is_convertible<typename IteratorTraits<T>::iterator_category, U>::value> {};

// 萃取某种迭代器
template <typename T, typename U>
struct HasIteratorCatOf<T, U, false> : public m_false_type {};

template <typename Iter>
struct IsInputIterator : public HasIteratorCatOf<Iter, InputIteratorTag> {};

template <typename Iter>
struct IsOutputIterator : public HasIteratorCatOf<Iter, OutputIteratorTag> {};

template <typename Iter>
struct IsForwardIterator : public HasIteratorCatOf<Iter, ForwardIteratorTag> {};

template <typename Iter>
struct IsBidirectionalIterator : public HasIteratorCatOf<Iter, BidirectionalIteratorTag> {};

template <typename Iter>
struct IsRandomAccessIterator : public HasIteratorCatOf<Iter, RandomAccessIteratorTag> {};

template <typename Iterator>
struct IsIterator : public m_bool_constant<
                        IsInputIterator<Iterator>::value || IsOutputIterator<Iterator>::value> {};

// 萃取某个迭代器的category
template <typename Iterator>
typename IteratorTraits<Iterator>::iterator_category iterator_category(const Iterator& /*unused*/) {
  typedef typename IteratorTraits<Iterator>::iterator_category Category;
  return Category();
}

// 萃取某个迭代器的distance_type
template <typename Iterator>
typename IteratorTraits<Iterator>::difference_type* distance_type(const Iterator& /*unused*/) {
  return static_cast<typename IteratorTraits<Iterator>::difference_type*>(0);
}

// 萃取某个迭代器的value_type
template <typename Iterator>
typename IteratorTraits<Iterator>::value_type* value_type(const Iterator& /*unused*/) {
  return static_cast<typename IteratorTraits<Iterator>::value_type*>(0);
}

// 以下函数用于计算迭代器之间的距离

// distance的InputIteratorTag的版本
template <typename InputIterator>
typename IteratorTraits<InputIterator>::difference_type distance_dispatch(
    InputIterator first, InputIterator last, InputIteratorTag /*unused*/) {
  typename IteratorTraits<InputIterator>::difference_type n = 0;
  while (first != last) {
    ++first;
    ++n;
  }
  return n;
}

// distance的RandomAccessIteratorTag的版本
template <typename RandomIterator>
typename IteratorTraits<RandomIterator>::difference_type distance_dispatch(
    RandomIterator first, RandomIterator last, RandomAccessIteratorTag /*unused*/) {
  return last - first;
}

template <typename InputIterator>
typename IteratorTraits<InputIterator>::difference_type distance(
    InputIterator first, InputIterator last) {
  return distance_dispatch(first, last, iterator_category(first));
}

// 以下函数用于让迭代器前进n个距离

// advance的InputIteratorTag版本
template <typename InputIterator, typename Distance>
void advance_dispatch(InputIterator& i, Distance n, InputIteratorTag /*unused*/) {
  while (n--) {
    ++i;
  }
}

// advance的BidirectionalIteratorTag的版本
template <typename BidirectionalIterator, typename Distance>
void advance_dispatch(BidirectionalIterator& i, Distance n, BidirectionalIteratorTag /*unused*/) {
  if (n >= 0) {
    while (n--) {
      ++i;
    }
  } else {
    while (n++) {
      --i;
    }
  }
}

// advance的RandomAccessIteratorTag的版本
template <typename RandomIter, typename Distance>
void advance_dispatch(RandomIter& i, Distance n, RandomAccessIteratorTag /*unused*/) {
  i += n;
}

template <typename InputIterator, typename Distance>
void advance(InputIterator& i, Distance n) {
  advance_dispatch(i, n, iterator_category(i));
}

// 模板类：reverse_iterator
// 反向迭代器
template <typename Iterator>
class ReverseIterator {
 private:
  Iterator current_;  // 对应的正向迭代器

 public:
  using iterator_category = typename IteratorTraits<Iterator>::iterator_category;
  using value_type = typename IteratorTraits<Iterator>::value_type;
  using difference_type = typename IteratorTraits<Iterator>::difference_type;
  using pointer = typename IteratorTraits<Iterator>::pointer;
  using reference = typename IteratorTraits<Iterator>::reference;

  using iterator_type = Iterator;
  using self = ReverseIterator<Iterator>;

  ReverseIterator() = default;
  explicit ReverseIterator(iterator_type i) : current_(i) {}
  ReverseIterator(const self& rhs) : current_(rhs.current_) {}

  iterator_type base() const { return current_; }

  reference operator*() const {
    auto tmp = current_;
    return *--tmp;
  }

  pointer operator->() const { return &(operator*()); }

  self& operator++() {
    --current_;
    return *this;
  }

  self operator++(int) {
    self tmp = *this;
    --current_;
    return tmp;
  }

  self& operator--() {
    ++current_;
    return *this;
  }

  self operator--(int) {
    self tmp = *this;
    ++current_;
    return tmp;
  }

  self& operator+=(difference_type n) {
    current_ -= n;
    return *this;
  }

  self operator+(difference_type n) const { return self(current_ - n); }

  self& operator-=(difference_type n) {
    current_ += n;
    return *this;
  }

  self operator-(difference_type n) const { return self(current_ + n); }

  reference operator[](difference_type n) const { return *(*this + n); };
};

// 重载operator-
template <typename Iterator>
typename ReverseIterator<Iterator>::difference_type operator-(
    const ReverseIterator<Iterator>& lhs, const ReverseIterator<Iterator>& rhs) {
  return rhs.base() - lhs.base();
}

// 重载比较操作符
template <typename Iterator>
bool operator==(const ReverseIterator<Iterator>& lhs, const ReverseIterator<Iterator>& rhs) {
  return lhs.base() == rhs.base();
}

template <typename Iterator>
bool operator<(const ReverseIterator<Iterator>& lhs, const ReverseIterator<Iterator>& rhs) {
  return rhs.base() < lhs.base();
}

template <typename Iterator>
bool operator!=(const ReverseIterator<Iterator>& lhs, const ReverseIterator<Iterator>& rhs) {
  return !(lhs == rhs);
}

template <typename Iterator>
bool operator>(const ReverseIterator<Iterator>& lhs, const ReverseIterator<Iterator>& rhs) {
  return rhs < lhs;
}

template <typename Iterator>
bool operator<=(const ReverseIterator<Iterator>& lhs, const ReverseIterator<Iterator>& rhs) {
  return !(rhs < lhs);
}

template <typename Iterator>
bool operator>=(const ReverseIterator<Iterator>& lhs, const ReverseIterator<Iterator>& rhs) {
  return !(lhs < rhs);
}

}  // namespace mystl

#endif  // !MYTINYSTL_ITERATOR_H_
