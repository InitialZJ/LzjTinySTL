#ifndef MYTINYSTL_ASTRING_H_
#define MYTINYSTL_ASTRING_H_

#include "basic_string.h"

namespace mystl {
using string = mystl::BasicString<char>;
using wstring = mystl::BasicString<wchar_t>;
using u16string = mystl::BasicString<char16_t>;
using u32string = mystl::BasicString<char32_t>;

}  // namespace mystl

#endif  // !MYTINYSTL_ASTRING_H_
