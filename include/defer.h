#pragma once

#include <functional>

template <typename _Func>
struct [[nodiscard]] defer {
  using function_type = _Func;
  function_type f;
#if __cplusplus >= 202002L
  constexpr 
#endif
  ~defer() { std::invoke(f); }
};

