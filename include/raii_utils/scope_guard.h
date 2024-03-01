#pragma once

#include <functional>

namespace raii {
namespace __details {
template <typename _Func>
struct [[nodiscard]] scope_guard {
  using function_type = _Func;
private:
  function_type _M_f;
public:
  scope_guard(_Func func) noexcept : _M_f(func) {}
  scope_guard(scope_guard const&) = delete;
  auto operator=(scope_guard const&) -> scope_guard& = delete;
#if __cplusplus >= 202002L
  constexpr 
#endif
  ~scope_guard() noexcept { std::invoke(_M_f); }
};
}
using __details::scope_guard;
}
