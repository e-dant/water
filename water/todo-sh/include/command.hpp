#pragma once

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace sh {

template <typename textual, typename buffer = std::array<char, 128>>
textual command(textual source) {
  buffer response;
  textual result;
  std::shared_ptr<FILE> pipe(popen(source, "r"), pclose);
  if (!pipe) throw std::runtime_error("popen() failed");
  while (!feof(pipe.get())) {
    if (fgets(response.data(), 128, pipe.get()) != nullptr)
      result += response.data();
  }
  return result;
}

}  // namespace sh

