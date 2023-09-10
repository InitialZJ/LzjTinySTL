#ifndef MYTINYSTL_SET_H_
#define MYTINYSTL_SET_H_

// 包含两个模板类 set 和 multiset
// set      : 集合，键值即实值，集合内元素会自动排序，键值不允许重复
// 对应书5.3节

// multiset : 集合，键值即实值，集合内元素会自动排序，键值允许重复

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
};

}  // namespace mystl

#endif  // ! MYTINYSTL_SET_H_