#include <chrono>
#include <iostream>
#include <thread>
#include <watcher.hpp>

namespace literals {
using ms = std::chrono::milliseconds;
using status = water::watcher::status;
using str = std::string;
using std::cout;
using std::endl;
using std::this_thread::sleep_for;
}  // namespace literals

int main() {
  using namespace literals;
  static constexpr auto root = ".";

  auto watcher = water::watcher(root);

  auto callback = [&](str hit, status stat) {
    static constexpr auto delay = ms{16};
    switch (stat) {
      case status::created:
        cout << "created: " << hit << endl;
        break;
      case status::modified:
        cout << "modified: " << hit << endl;
        break;
      case status::erased:
        cout << "erased: " << hit << endl;
        break;
      default:
        cout << "unknown: " << hit << endl;
    }
    sleep_for(delay);
  };

  while (true)
    watcher.run(callback);

  return 0;
}
