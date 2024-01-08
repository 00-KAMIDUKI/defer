#include <unique.h>

void f(int&) {}
using t = raii::unique_handle<int>;
static auto test(t& i) {
  auto const i1 = t{};
  auto i2 = t{1};
  auto i3 = raii::unique_handle<int, decltype([](auto) {})>{1};
  static_assert(std::is_same_v<decltype(i1.get()), int const&>);
  f(i.get());
}

static auto test() {
  auto i = raii::unique<>{};
}

static bool defer_called = false;
int main() { 
  {
    auto defer = raii::unique_defer{[]() { defer_called = true; }};
  }
  if (!defer_called) {
    return 1;
  }
  return 0;
}
