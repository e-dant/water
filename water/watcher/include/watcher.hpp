#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace water {
using json = nlohmann::json;

class watcher {
 public:
  enum class status { created, modified, erased };
  bool is_erased, is_created, is_modified = false;
  std::string path_to_watch;
  /** @brief watcher/constructor
   *  @param path - path to monitor for
   *  @see watcher::status
   *  Creates a file map from the
   *  given path.
   **/
  watcher(std::string root = ".") : path_to_watch{root} {
    if (std::filesystem::is_directory(root)) {
      for (const auto& file :
           std::filesystem::recursive_directory_iterator(
               root)) {
        file_map[file.path().string()] =
            std::filesystem::last_write_time(file);
      }
    } else {
      file_map[root] =
          std::filesystem::last_write_time(root);
    }
  }

  /** @brief watcher/run
   *  @param closure - action to perform on
   *  status "ticks"
   *  Monitors `path_to_watch` for changes.
   *  Executes the given closure when they
   *  happen.
   **/
  template <typename T = void>
  void run(
      const std::function<T(std::string, status)> action) {
    is_erased = false;
    is_created = false;
    is_modified = false;
    auto it = file_map.begin();
    while (it != file_map.end()) {
      // File erased
      if (!std::filesystem::exists(it->first)) {
        action(it->first, status::erased);
        it = file_map.erase(it);
        is_erased = true;
      } else {
        it++;
      }
    }
    // Check if a file was created or modified
    // If path is a directory:
    if (std::filesystem::is_directory(path_to_watch)) {
      for (const auto& file :
           std::filesystem::recursive_directory_iterator(
               path_to_watch)) {
        const auto current_file_last_write_time =
            std::filesystem::last_write_time(file);
        // File created
        if (!file_map.contains(file.path().string())) {
          file_map[file.path().string()] =
              current_file_last_write_time;
          action(file.path().string(), status::created);
          is_created = true;
        }
        // File modified
        else {
          if (file_map[file.path().string()] !=
              current_file_last_write_time) {
            file_map[file.path().string()] =
                current_file_last_write_time;
            action(file.path().string(), status::modified);
            is_modified = true;
          }
        }
      }
      // If path is a file:
    } else {
      auto current_file_last_write_time =
          std::filesystem::last_write_time(path_to_watch);
      // File created
      if (!file_map.contains(path_to_watch)) {
        file_map[path_to_watch] =
            current_file_last_write_time;
        action(path_to_watch, status::created);
        is_created = true;
      }
      // File modified
      else {
        if (file_map[path_to_watch] !=
            current_file_last_write_time) {
          file_map[path_to_watch] =
              current_file_last_write_time;
          action(path_to_watch, status::modified);
          is_modified = true;
        }
      }
    }
  }

 private:
  std::unordered_map<std::string,
                     std::filesystem::file_time_type>
      file_map;
};

}  // namespace water
