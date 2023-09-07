#ifndef MYTINYSTL_STACK_H_
#define MYTINYSTL_STACK_H_

// stack：栈
// 对应书4.5节

#include "deque.h"

namespace mystl {
// 模板类Stack
// 参数一代表数据类型，参数二代表底层容器类型，缺省使用mystl::deque作为底层容器
template <typename T, typename Container = mystl::Deque<T>>
class Stack {
 public:
  using container_type = Container;
  using value_type = typename Container::value_type;
  using size_type = typename Container::size_type;
  using reference = typename Container::reference;
  using const_reference = typename Container::const_reference;
};

}  // namespace mystl

#endif  // ! MYTINYSTL_STACK_H_
