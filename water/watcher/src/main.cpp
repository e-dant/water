#include <chrono>
#include <watcher.hpp>

namespace literals {
using ms = std::chrono::milliseconds;
using status = water::watcher::status;
using str = std::string;
}  // namespace literals

int main() {
  using namespace literals;

  static constexpr auto root = ".";

  auto watcher = water::watcher(root);

  while (true)
    watcher.run();

  return 0;
}
