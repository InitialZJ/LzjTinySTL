#ifndef MYTINYSTL_CONSTRUCT_H_
#define MYTINYSTL_CONSTRUCT_H_

// 负责对象的构造和析构
// 对应书2.2.3节

#include <new>

#include "iterator.h"
#include "type_traits.h"
#include "util.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4100)  // unused parameter
#endif                           // _MSC_VER

namespace mystl {

// 构造
template <typename Ty>
void construct(Ty* ptr) {
  ::new ((void*)ptr) Ty();
}

template <typename Ty1, typename Ty2>
void construct(Ty1* ptr, const Ty2& value) {
  ::new ((void*)ptr) Ty1(value);
}

template <typename Ty, typename... Args>
void construct(Ty* ptr, Args&&... args) {
  ::new ((void*)ptr) Ty(mystl::forward<Args>(args)...);
}

// 析构

template <typename Ty>
void destroy_one(Ty* /*unused*/, std::true_type /*unused*/) {}

template <typename Ty>
void destroy_one(Ty* pointer, std::false_type /*unused*/) {
  if (pointer != nullptr) {
    pointer->~Ty();
  }
}

template <typename ForwardIter>
void destroy_cat(ForwardIter /*unused*/, ForwardIter /*unused*/, std::true_type /*unused*/) {}

template <typename ForwardIter>
void destroy_cat(ForwardIter first, ForwardIter last, std::false_type /*unused*/) {
  for (; first != last; ++first) {
    destroy(&*first);
  }
}

template <typename Ty>
void destroy(Ty* pointer) {
  destroy_one(pointer, std::is_trivially_destructible<Ty>{});
}

template <class ForwardIter>
void destroy(ForwardIter first, ForwardIter last) {
  destroy_cat(
      first,
      last,
      std::is_trivially_destructible<typename IteratorTraits<ForwardIter>::value_type>{});
}

}  // namespace mystl

#ifdef _MSC_VER
#pragma warning(pop)
#endif  // _MSC_VER

#endif  // !MYTINYSTL_CONSTRUCT_H_
