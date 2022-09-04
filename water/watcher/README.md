# Watcher


## Summary

The source code and build system for *Watcher*,
an arbitrary path watcher.

## Usage

### Brief

```cpp
// at some point, call 'run'
// with some millisecond delay,
// it's template parameter.
// the default it 16 ms.
water::watcher::run<16>(
  // scan some path, forever...
  ".",
  // providing a callback
  // that does whatever you'd like...
  [](
    const water::concepts::Path auto file,
    const water::watcher::status s) {
      const auto pf = [&file](const auto s) {
        std::cout << s << ": " << file << std::endl;
      };
      // such as printing what happened.
      switch (s) {
        case created:
          pf("created");
          break;
        case modified:
          pf("modified");
          break;
        case erased:
          pf("erased");
          break;
        default:
          pf("unknown");
      }
      });
```

### Long

```cpp
#include <chrono>
#include <iostream>
#include <thread>
#include <type_traits>
#include <watcher.hpp>

using namespace water;
using namespace watcher;
using namespace concepts;

const auto stutter_print
    = [](const Path auto file, const status s)
  {

  using status::created, status::modified, status::erased,
      std::endl, std::cout;

  const auto pf = [&file](const auto s) {
    cout << s << ": " << file << endl;
  };

  switch (s) {
    case created:
      pf("created");
      break;
    case modified:
      pf("modified");
      break;
    case erased:
      pf("erased");
      break;
    default:
      pf("unknown");
  }
};

int main(int argc, char** argv) {
  auto path = argc > 1
                  // we have been given a path,
                  // and we will use it
                  ? argv[1]
                  // otherwise, default to the
                  // current directory
                  : ".";
  return run(
      // scan the path, forever...
      path,
      // printing what we find,
      // every 16 milliseconds.
      stutter_print);
}
```

## Content

- `watcher.hpp`:
    Include this to use it elsewhere.
- `main.cpp`:
    Build this to use it as a CLI program.

## Building & Running

If you use the `tell` program,
which is completely optional:

```sh
cd water/water/watcher
`tell build`
# watches the current directory
`tell run`
# watches some given path
# `tell run` 'your/favorite/path'
```

This will take care of:
  - building a compiler if one is not found
  - linking the `compile_commands.json` file
    to this project's root.

Otherwise, build it with `cmake`:

```sh
cd water/water/watcher/build
cmake -S 'in' -B 'out' -G 'Ninja' -Wno-dev
cmake --build out --config Release
cd out
# watches the current directory
./water.watcher
# watches some given path
# ./water.watcher 'your/favorite/path'
```