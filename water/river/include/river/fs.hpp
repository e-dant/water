#pragma once

#include <filesystem>
#include <iostream>

namespace river {
namespace fs {

// move signature
template <class Path>
auto rprint(const Path&& base = ".") noexcept -> void {
  for (auto const& path : std::filesystem::recursive_directory_iterator{base})
    std::cout << path.path() << std::endl;
}

// copy signature
template <class Path>
auto rprint(const Path& base = ".") noexcept -> void {
  for (auto const& path : std::filesystem::recursive_directory_iterator{base})
    std::cout << path.path() << std::endl;
}

// do not allow `exists` and `relpath` to be copy-constructed
// because the arguments might allocate
template <class Path>
auto exists(const Path&& path, const Path&& base = ".") noexcept -> bool {
  const auto&& stat = std::filesystem::file_status{};
  if (std::filesystem::status_known(stat)
          ? std::filesystem::exists(stat)
          : std::filesystem::exists(base.generic_string() +
                                    path.generic_string()))
    return true;
  else
    return false;
}

// do not allow `exists` and `relpath` to be copy-constructed
// because the arguments might allocate
template <class Path>
auto relpath(const Path&& path, const Path&& base = ".") noexcept -> auto{
  if (river::fs::exists(std::move(path))) {
    return base.generic_string() + path.generic_string();
  } else {
    return base.generic_string();
  }
}

}  // namespace fs
}  // namespace river
