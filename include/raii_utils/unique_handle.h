#pragma once

#include <functional>

namespace raii {
namespace __details {

template <typename _T, _T __null_element>
struct nullable_wrapper {
  using value_type = _T;
private:
  value_type _M_value;
public:
  constexpr nullable_wrapper(value_type const& __value) noexcept
    : _M_value{__value} {}
  constexpr nullable_wrapper(value_type&& __value) noexcept
    : _M_value{std::move(__value)} {}
  constexpr nullable_wrapper() noexcept 
    : _M_value{__null_element} {}

  constexpr auto value() const noexcept 
    -> value_type const& { return _M_value; }
  constexpr auto value() noexcept 
    -> value_type& { return _M_value; }

  constexpr auto has_value() const noexcept 
    -> bool { return _M_value != __null_element; }

  constexpr void reset() noexcept 
  { _M_value = __null_element; }
};

template <typename _T>
concept nullable = requires(_T& __mutable, _T const& __constant) {
  { __constant.has_value() } -> std::convertible_to<bool>;
  __mutable.reset();
  { __constant.value() } -> std::convertible_to<typename _T::value_type const&>;
};

struct __void_nullable_adapter: nullable_wrapper<bool, false> {
  constexpr __void_nullable_adapter() noexcept
    : nullable_wrapper<bool, false>(true) { struct value_type {}; }
};

template <typename _T>
struct __default_deleter {
  constexpr void operator()(_T const&) const noexcept {}
};

template <>
struct __default_deleter<__void_nullable_adapter::value_type> {
  constexpr void operator()() const noexcept {}
};

template <nullable _T=__void_nullable_adapter, typename _Deleter=__default_deleter<typename _T::value_type>>
struct unique {
  using value_type = typename _T::value_type;
  static_assert(std::is_reference_v<value_type> == false);
private:
  [[no_unique_address]] _T _M_value;
  [[no_unique_address]] _Deleter _M_deleter;

public:
  constexpr unique(value_type const& __value, _Deleter __deleter) noexcept
    : _M_value{__value}
    , _M_deleter{__deleter} {}

  constexpr unique(value_type&& __value, _Deleter __deleter) noexcept
    : _M_value{std::move(__value)}
    , _M_deleter{__deleter} {}

  constexpr unique(value_type const& __value) noexcept
    requires (!std::is_pointer_v<_Deleter>)
    : _M_value{__value}
    , _M_deleter{} {}

  constexpr unique(value_type&& __value) noexcept
    requires (!std::is_pointer_v<_Deleter>)
    : _M_value{std::move(__value)}
    , _M_deleter{} {}

  constexpr unique(_Deleter __deleter) noexcept
    : _M_value{}
    , _M_deleter{__deleter} {}

  constexpr unique() noexcept
    requires (!std::is_pointer_v<_Deleter>)
    : _M_value{}
    , _M_deleter{} {}

  unique(unique const&) = delete;
  auto operator=(unique const&) -> unique& = delete;

  constexpr unique(unique&& __other) noexcept
    : _M_value{std::move(__other._M_value)}
    , _M_deleter{std::move(__other._M_deleter)} 
    { __other._M_value.reset(); }

  constexpr auto operator=(unique&& __other) noexcept -> unique& {
    if (this != &__other) {
      _M_value = std::move(__other._M_value);
      _M_deleter = std::move(__other._M_deleter);
      __other._M_value.reset();
    }
    return *this;
  }

  constexpr ~unique() noexcept
  requires (!std::is_same_v<_T, __void_nullable_adapter>) {
    if (_M_value.has_value()) {
      std::invoke(_M_deleter, _M_value.value());
    }
  }

  constexpr ~unique() noexcept 
  requires (std::is_same_v<_T, __void_nullable_adapter>) {
    if (_M_value.has_value()) {
      std::invoke(_M_deleter);
    }
  }

  constexpr auto get() const noexcept 
    -> value_type const&
  requires (!std::is_same_v<_T, __void_nullable_adapter>)
  { return _M_value.value(); }

  constexpr auto get() noexcept 
    -> value_type& 
  requires (
    requires { { _M_value.value() } -> std::convertible_to<typename _T::value_type&>; }
    && !std::is_same_v<_T, __void_nullable_adapter>
  ) { return _M_value.value(); }

  constexpr void reset() noexcept {
    if (_M_value.has_value()) {
      std::invoke(_M_deleter, _M_value.value());
      _M_value.reset();
    }
  }

  constexpr void leak() noexcept 
  { _M_value.reset(); }
};

template <typename _Func>
struct scope_guard: unique<__void_nullable_adapter, _Func> {};

template <typename _Func>
scope_guard(_Func) -> scope_guard<_Func>;

template <typename _T, typename _Deleter=__default_deleter<_T>, auto __null_element=0>
using unique_handle = unique<nullable_wrapper<_T, __null_element>, _Deleter>;

}
using __details::nullable;
using __details::nullable_wrapper;
using __details::unique;
using __details::scope_guard;
using __details::unique_handle;
}

