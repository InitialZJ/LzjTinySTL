#ifndef MYTINYSTL_MEMORY_H_
#define MYTINYSTL_MEMORY_H_

// 负责更高级的动态内存管理
// 包含一些基本函数、空间配置器、未初始化的储存空间管理，以及一个模板类auto_ptr

#include <climits>
#include <cstddef>
#include <cstdlib>

#include "algobase.h"
#include "allocator.h"
#include "construct.h"
#include "uninitialized.h"

namespace mystl {

// 获取对象地址
template <typename Tp>
constexpr Tp* address_of(Tp& value) noexcept {
  return &value;
}

}  // namespace mystl

#endif  // !MYTINYSTL_MEMORY_H_