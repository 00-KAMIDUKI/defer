#pragma once

#include <functional>

namespace raii {
namespace __details {
template <typename _Func>
struct [[nodiscard]] defer {
  using function_type = _Func;
private:
  function_type _M_f;
public:
  defer(_Func func) noexcept : _M_f(func) {}
  defer(defer const&) = delete;
  auto operator=(defer const&) -> defer& = delete;
#if __cplusplus >= 202002L
  constexpr 
#endif
  ~defer() noexcept { std::invoke(_M_f); }
};
}
using __details::defer;
}
