#include <watcher.hpp>

int main(int argc, char** argv) {
  using namespace water;

  if (argc > 1)
    watcher::populate(argv[1]);
  else
    watcher::populate(".");
  while(true)
    watcher::run();

  return 0;
}
