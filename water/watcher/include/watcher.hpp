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

using json = nlohmann::json;

enum class FileStatus {
  created,
  modified,
  erased
};
class FileWatcher {
public:
  bool file_erased = false;
  bool file_created = false;
  bool file_modified = false;
  std::string path_to_watch;
  // Constructor:
  //      1. create array variable file_map, and
  //      within it:
  //      2. keep a record of files from the base
  //      directory and their last modification
  //      time (last_write_time)
  FileWatcher(std::string path_to_watch)
      : path_to_watch{path_to_watch} {
    if (std::filesystem::is_directory(
            path_to_watch)) {
      for (auto &file : std::filesystem::
               recursive_directory_iterator(
                   path_to_watch)) {
        file_map[file.path().string()] =
            std::filesystem::last_write_time(
                file);
      }
    } else {
      file_map[path_to_watch] =
          std::filesystem::last_write_time(
              path_to_watch);
    }
  }
  // Monitor "path_to_watch" for changes and in
  // case of a change execute the user supplied
  // "action" (lambda) function
  void
  scan(const std::function<
       void(std::string, FileStatus)> &action) {
    file_erased = false;
    file_created = false;
    file_modified = false;
    auto it = file_map.begin();
    while (it != file_map.end()) {
      // File erased
      if (!std::filesystem::exists(it->first)) {
        action(it->first, FileStatus::erased);
        it = file_map.erase(it);
        file_erased = true;
      } else {
        it++;
      }
    }
    // Check if a file was created or modified
    // If path is a directory:
    if (std::filesystem::is_directory(
            path_to_watch)) {
      for (auto &file : std::filesystem::
               recursive_directory_iterator(
                   path_to_watch)) {
        auto current_file_last_write_time =
            std::filesystem::last_write_time(
                file);
        // File created
        if (!file_map.contains(
                file.path().string())) {
          file_map[file.path().string()] =
              current_file_last_write_time;
          action(file.path().string(),
                 FileStatus::created);
          file_created = true;
        }
        // File modified
        else {
          if (file_map[file.path().string()] !=
              current_file_last_write_time) {
            file_map[file.path().string()] =
                current_file_last_write_time;
            action(file.path().string(),
                   FileStatus::modified);
            file_modified = true;
          }
        }
      }
      // If path is a file:
    } else {
      auto current_file_last_write_time =
          std::filesystem::last_write_time(
              path_to_watch);
      // File created
      if (!file_map.contains(path_to_watch)) {
        file_map[path_to_watch] =
            current_file_last_write_time;
        action(path_to_watch,
               FileStatus::created);
        file_created = true;
      }
      // File modified
      else {
        if (file_map[path_to_watch] !=
            current_file_last_write_time) {
          file_map[path_to_watch] =
              current_file_last_write_time;
          action(path_to_watch,
                 FileStatus::modified);
          file_modified = true;
        }
      }
    }
  }

private:
  std::unordered_map<
      std::string,
      std::filesystem::file_time_type>
      file_map;
};
