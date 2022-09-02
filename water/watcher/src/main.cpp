#include <chrono>
#include <iostream>
#include <thread>
#include <watcher.hpp>

int main() {
  using namespace water;

  std::cout << "watching..." << std::endl;

  static constexpr const auto poll_delay =
      std::chrono::milliseconds(100);

  auto w = watcher{"."};

  while (true) {
    w.run<void>([&](std::string path_to_watch,
               watcher::status status) -> void {
      switch (status) {
        case watcher::status::created:
          std::cout
              << "File created: " << path_to_watch
              << '\n';
          break;
        case watcher::status::modified:
          std::cout << "File modified: "
                    << path_to_watch << '\n';
          break;
        case watcher::status::erased:
          std::cout
              << "File erased: " << path_to_watch
              << '\n';
          break;
        default:
          std::cout
              << "Error! Unknown file status.\n";
      }
    });

    std::this_thread::sleep_for(poll_delay);
  }

  return 0;
}
