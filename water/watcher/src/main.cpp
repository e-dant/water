#include <chrono>
#include <iostream>
#include <thread>
#include <type_traits>
#include <watcher.hpp>

using namespace water;
using namespace watcher;
using namespace concepts;

template <const auto delay_ms = 0>
const auto stutter_print = [](const Path auto file,
                              const status s) requires
    std::is_integral_v<decltype(delay_ms)> {
  using status::created, status::modified, status::erased,
      std::endl, std::cout, std::this_thread::sleep_for,
      std::chrono::milliseconds;

  switch (s) {
    case created:
      cout << "created: " << file << endl;
      break;
    case modified:
      cout << "modified: " << file << endl;
      break;
    case erased:
      cout << "erased: " << file << endl;
      break;
    default:
      cout << "unknown: " << file << endl;
  }

  if constexpr (delay_ms > 0)
    sleep_for(milliseconds(delay_ms));
};

void run_loop(const Path auto path) {
  // Because I'm hoping that this is
  // the first C++ program to use concepts
  // and goto in the same (main) file
top:
  run(path, stutter_print<16>);
  goto top;  // lol
};

int main(int argc, char** argv) {
  auto path = argc > 1 ? argv[1] : ".";

  populate(path);

  run_loop(path);

  // while (true)
  //   run(path, stutter_print<16>);

  return 0;
}
