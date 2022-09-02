#include <chrono>
#include <iostream>
#include <thread>
#include <watcher.hpp>

int main() {
  using namespace water;
  //static constexpr const auto root = ".";
  static constexpr const auto delay =
      std::chrono::milliseconds(16);

  auto w = watcher();

  while (true) {
    w.run(
        [&](std::string hit, watcher::status status) {
          switch (status) {
            case watcher::status::created:
              std::cout << "created: " << hit << std::endl;
              break;
            case watcher::status::modified:
              std::cout << "modified: " << hit << std::endl;
              break;
            case watcher::status::erased:
              std::cout << "erased: " << hit << std::endl;
              break;
            default:
              std::cout << "unknown: " << hit << std::endl;
          }
        });

    std::this_thread::sleep_for(delay);
  }

  return 0;
}
