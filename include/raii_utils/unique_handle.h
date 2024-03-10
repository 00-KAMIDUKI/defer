#pragma once

#include <functional>

namespace raii {
namespace __details {

template <typename _T, _T __null_element>
struct nullable_wrapper {
  using value_type = _T;
private:
  value_type _M_value{__null_element};
public:
  constexpr nullable_wrapper(value_type const& __value) noexcept
    : _M_value{__value} {}
  constexpr nullable_wrapper(value_type&& __value) noexcept
    : _M_value{std::move(__value)} {}
  constexpr nullable_wrapper() noexcept = default;

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
  std::tuple<_T, _Deleter> _M_tuple;
  
  auto& _M_value_wrapper() noexcept { return std::get<0>(_M_tuple); }
  auto const& _M_value_wrapper() const noexcept { return std::get<0>(_M_tuple); }
  auto& _M_deleter() noexcept { return std::get<1>(_M_tuple); }
  auto const& _M_deleter() const noexcept { return std::get<1>(_M_tuple); }
public:
  constexpr unique(value_type const& __value, auto&& __deleter) noexcept
    : _M_tuple{_T{__value}, std::forward<decltype(__deleter)>(__deleter)} {}

  constexpr unique(value_type&& __value, auto&& __deleter) noexcept
    : _M_tuple{_T{std::move(__value)}, std::forward<decltype(__deleter)>(__deleter)} {}

  explicit constexpr unique(auto&& __deleter) noexcept
    requires (std::is_default_constructible_v<_T>)
    : _M_tuple{_T{}, std::forward<decltype(__deleter)>(__deleter)} {}

  constexpr unique(value_type const& __value) noexcept
    requires (!std::is_pointer_v<_Deleter> && std::is_default_constructible_v<_Deleter>)
    : _M_tuple{__value, _Deleter{}} {}

  constexpr unique(value_type&& __value) noexcept
    requires (!std::is_pointer_v<_Deleter> && std::is_default_constructible_v<_Deleter>)
    : _M_tuple{std::move(__value), _Deleter{}} {}

  constexpr unique() noexcept
    requires (
      std::is_default_constructible_v<_T>
      && !std::is_pointer_v<_Deleter> && std::is_default_constructible_v<_Deleter>
    ): _M_tuple{_T{}, _Deleter{}} {}

  unique(unique const&) = delete;
  auto operator=(unique const&) -> unique& = delete;

  constexpr unique(unique&& __other) noexcept
    :_M_tuple{std::move(__other._M_tuple)}
    { __other._M_value_wrapper().reset(); }

  constexpr auto operator=(unique&& __other) noexcept -> unique& {
    if (this != &__other) {
      this->_M_tuple = std::move(__other._M_tuple);
      __other._M_value_wrapper().reset();
    }
    return *this;
  }

  constexpr ~unique() noexcept
  requires (!std::is_same_v<_T, __void_nullable_adapter>) {
    if (this->_M_value_wrapper().has_value()) {
      std::invoke(this->_M_deleter(), this->_M_value_wrapper().value());
    }
  }

  constexpr ~unique() noexcept 
  requires (std::is_same_v<_T, __void_nullable_adapter>) {
    if (this->_M_value_wrapper().has_value()) {
      std::invoke(this->_M_deleter());
    }
  }

  constexpr auto get() const noexcept 
    -> value_type const&
  requires (!std::is_same_v<_T, __void_nullable_adapter>)
  { return this->_M_value_wrapper().value(); }

  constexpr auto get() noexcept 
    -> value_type& 
  requires (
    requires { { this->_M_value_wrapper().value() } -> std::convertible_to<typename _T::value_type&>; }
    && !std::is_same_v<_T, __void_nullable_adapter>
  ) { return this->_M_value_wrapper().value(); }

  constexpr void reset() noexcept {
    if (this->_M_value_wrapper().has_value()) {
      std::invoke(this->_M_deleter(), this->_M_value_wrapper().value());
      this->_M_value_wrapper().reset();
    }
  }

  constexpr void leak() noexcept 
  { this->_M_value_wrapper().reset(); }
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

