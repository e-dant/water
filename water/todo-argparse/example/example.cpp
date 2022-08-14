#include <iostream>
#include "../include/argparse.hpp"
using std::cout;
using std::endl;
int main(int argc, char **argv) try {
  argparse args{argc, argv};
  cout << args.get("--something").value_or("strange arguments") << endl;
  return args.has("--something");
} catch (std::exception const &e) {
  cout << "uh oh... " << e.what();
  std::exit(1);
}
