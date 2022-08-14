#include <algorithm>
#include <iostream>
#include <iterator>
#include <optional>
#include <ostream>
#include <string>
#include <vector>

using std::find;
using std::optional;
using std::string;
using std::vector;
using std::move;
using std::cout;
using std::endl;

class argparse {
  private:
  vector<string> tokens;
  public:
  argparse(vector<string> &&arguments) {
    for (auto it : arguments) tokens.push_back(it);
  }
  argparse(auto argc, char **argv) {
    for (auto i = 0; i < argc; i++) tokens.push_back(string(argv[i]));
  }

  auto get(string&& option) const -> optional<string> {
      auto found = find(begin(tokens), end(tokens), option);
      if (found != end(tokens)) {
        std::advance(found, 1);
        return move(*found);
      }
      return nullptr;
  }

  auto has(std::string&& option) const {
    return move(find(begin(tokens), end(tokens), option) != end(tokens) ? false
                                                                        : true);
  }

  auto print() const {
    for (auto it : tokens) cout << it << endl;
  }
};
