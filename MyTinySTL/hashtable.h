#ifndef MYTINYSTL_HASHTABLE_H_
#define MYTINYSTL_HASHTABLE_H_

// 这个头文件包含了一个模板类 hashtable
// hashtable : 哈希表，使用开链法处理冲突
// 对应书5.7节

#include <initializer_list>

#include "algo.h"
#include "exceptdef.h"
#include "functional.h"
#include "iterator.h"
#include "memory.h"
#include "type_traits.h"
#include "util.h"
#include "vector.h"

namespace mystl {

// 结点定义
template <typename T>
struct HashtableNode {
  HashtableNode* next;  // 指向下一结点
  T value;              // 储存实值

  HashtableNode() = default;
  HashtableNode(const T& n) : next(nullptr), value(n) {}

  HashtableNode(const HashtableNode& node) : next(node.next), value(node.value) {}
  HashtableNode(HashtableNode&& node) : next(node.next), value(mystl::move(node.value)) {
    node.next = nullptr;
  }
};

// value traits
template <typename T, bool>
struct HtValueTraitsImp {
  using key_type = T;
  using mapped_type = T;
  using value_type = T;

  template <typename Ty>
  static const key_type& get_key(const Ty& value) {
    return value;
  }

  template <typename Ty>
  static const value_type& get_value(const Ty& value) {
    return value;
  }
};

template <typename T>
struct HtValueTraitsImp<T, true> {
  using key_type = typename std::remove_cv<typename T::first_type>::type;
  using mapped_type = typename T::second_type;
  using value_type = T;

  template <typename Ty>
  static const key_type& get_key(const Ty& value) {
    return value.first;
  }

  template <typename Ty>
  static const value_type& get_value(const Ty& value) {
    return value;
  }
};

template <typename T>
struct HtValueTraits {
  static constexpr bool kIsMap = mystl::IsPair<T>::kValue;

  using value_traits_type = HtValueTraitsImp<T, kIsMap>;

  using key_type = typename value_traits_type::key_type;
  using mapped_type = typename value_traits_type::mapped_type;
  using value_type = typename value_traits_type::value_type;

  template <typename Ty>
  static const key_type& get_key(const Ty& value) {
    return value_traits_type::get_key(value);
  }

  template <typename Ty>
  static const value_type& get_value(const Ty& value) {
    return value_traits_type::get_value(value);
  }
};

// forward declaration
template <typename T, typename HashFun, typename KeyEqual>
class Hashtable;

template <typename T, typename HashFun, typename KeyEqual>
struct HtIterator;

template <typename T, typename HashFun, typename KeyEqual>
struct HtConstIterator;

template <typename T>
struct HtLocalIterator;

template <typename T>
struct HtConstLocalIterator;

// ht_iterator
template <typename T, typename Hash, typename KeyEqual>
struct HtIteratorBase : public mystl::Iterator<mystl::ForwardIteratorTag, T> {
  using hashtable = mystl::Hashtable<T, Hash, KeyEqual>;
  using base = HtIteratorBase<T, Hash, KeyEqual>;
  using iterator = mystl::HtIterator<T, Hash, KeyEqual>;
  using const_iterator = mystl::HtConstIterator<T, Hash, KeyEqual>;
  using node_ptr = HashtableNode<T>*;
  using contain_ptr = hashtable*;
  using const_node_ptr = const node_ptr;
  using const_container_ptr = const contain_ptr;

  using size_type = size_t;
  using difference_type = ptrdiff_t;

  node_ptr node;   // 迭代器当前所指结点
  contain_ptr ht;  // 保持与容器的连接

  HtIteratorBase() = default;

  bool operator==(const base& rhs) const { return node == rhs.node; }
  bool operator!=(const base& rhs) const { return node != rhs.node; }
};

template <typename T, typename Hash, typename KeyEqual>
struct HtIterator : public HtIteratorBase<T, Hash, KeyEqual> {
  using base = HtIteratorBase<T, Hash, KeyEqual>;
  using hashtable = typename base::hashtable;
  using iterator = typename base::iterator;
  using const_iterator = typename base::const_iterator;
  using node_ptr = typename base::node_ptr;
  using contain_ptr = typename base::contain_ptr;

  using value_traits = HtValueTraits<T>;
  using value_type = T;
  using pointer = value_type*;
  using reference = value_type&;

  using base::ht;
  using base::node;

  HtIterator() = default;
  HtIterator(node_ptr n, contain_ptr t) {
    node = n;
    ht = t;
  }
}

}  // namespace mystl
#endif  // !MYTINYSTL_HASHTABLE_H_
