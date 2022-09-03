#include <chrono>
#include <iostream>
#include <thread>
#include <watcher.hpp>

using namespace water;
using namespace watcher;
using namespace concepts;

template <auto delay_ms = 0>
auto stutter_print = [](const Path auto file, status s) {
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

int main(int argc, char** argv) {
  auto path = argc > 1 ? argv[1] : ".";

  populate(path);

  while (true)
    run(path, stutter_print<16>);

  return 0;
}
