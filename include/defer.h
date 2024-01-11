#pragma once

#include <functional>

namespace raii {
namespace __details {
template <typename _Func>
struct [[nodiscard]] defer {
  using function_type = _Func;
  // static_assert(std::is_nothrow_invocable_v<function_type>);
  // TODO: make this field private
  function_type _M_f;
#if __cplusplus >= 202002L
  constexpr 
#endif
  ~defer() noexcept { std::invoke(_M_f); }
};
}
using __details::defer;
}
