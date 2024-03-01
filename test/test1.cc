#include <raii_utils/unique_handle.h>
#include <raii_utils/defer.h>

void f(int&) {}
using t = raii::unique_handle<int>;
[[maybe_unused]] static auto test(t& i) {
  auto const i1 = t{};
  auto i2 = t{1};
  auto i3 = raii::unique_handle<int, decltype([](auto) {})>{1};
  static_assert(std::is_same_v<decltype(i1.get()), int const&>);
  f(i.get());
}

[[maybe_unused]] static auto test() {
  auto i = raii::unique<>{};
  raii::defer _{[] {}};
}

static auto test1() {
  static bool defer_called = false;
  {
    auto defer = raii::scope_guard{[]() { defer_called = true; }};
    static_assert(sizeof defer == 1);
  }
  if (!defer_called) {
    std::exit(1);
  }
}

#include <type_traits>

static auto test2() {
  static bool deleter_called = false;
  {
    auto deleter = [](auto) { deleter_called = true; };
    raii::unique_handle<int, decltype(deleter)> i{1};
    static_assert(sizeof i == 4);
    static_assert(std::is_same_v<decltype(i)::value_type, int>);
  }
  if (!deleter_called) {
    std::exit(1);
  }
}

int main() { 
  test1();
  test2();
}
