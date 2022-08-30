#include <chrono>
#include <iostream>
#include <thread>
#include <watcher.hpp>

int main() {
  std::cout << "watching..." << std::endl;

  constexpr const auto poll_delay_ns =
      std::chrono::milliseconds(100);

  auto w = FileWatcher(".");

  while (true) {
    w.scan([&](std::string path_to_watch,
               FileStatus status) -> void {
      switch (status) {
      case FileStatus::created:
        std::cout
            << "File created: " << path_to_watch
            << '\n';
        break;
      case FileStatus::modified:
        std::cout
            << "File modified: " << path_to_watch
            << '\n';
        break;
      case FileStatus::erased:
        std::cout
            << "File erased: " << path_to_watch
            << '\n';
        break;
      default:
        std::cout
            << "Error! Unknown file status.\n";
      }
    });

    std::this_thread::sleep_for(poll_delay_ns);
  }

  return 0;
}
