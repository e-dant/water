#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace water {

class watcher {
 public:
  enum class status { created, modified, erased };

  std::string path_to_watch;
  /** @brief watcher/constructor
   *  @param path - path to monitor for
   *  @see watcher::status
   *  Creates a file map from the
   *  given path.
   **/
  watcher(std::string root = ".") : path_to_watch{root} {
    using namespace std::filesystem;
    if (is_directory(root)) {
      for (const auto& file :
           recursive_directory_iterator(root)) {
        bucket[file.path().string()] =
            last_write_time(file);
      }
    } else {
      bucket[root] = last_write_time(root);
    }
  }

  /** @brief watcher/run
   *  @param closure - action to perform on
   *  status "ticks"
   *  Monitors `path_to_watch` for changes.
   *  Executes the given closure when they
   *  happen.
   **/
  void run(const std::function<void(std::string, status)>
               action) {
    using namespace std::filesystem;
    auto it = bucket.begin();
    // first of all
    while (it != bucket.end()) {
      // check if this thing even exists
      if (!exists(it->first)) {
        // if not, call the closure on it,
        // indicating erasure
        action(it->first, status::erased);
        // and removing it from our map
        it = bucket.erase(it);
      } else {
        it++;
      }
    }
    // if this thing is a directory
    if (is_directory(path_to_watch)) {
      // iterate through its contents
      for (const auto& file :
           recursive_directory_iterator(path_to_watch)) {
        // grabbing the last write times
        const auto current_file_last_write_time =
            last_write_time(file);
        // checking if they're in our map
        if (!bucket.contains(file.path().string())) {
          // putting them there if not
          bucket[file.path().string()] =
              current_file_last_write_time;
          // and calling the closure on them,
          // indicating creation
          action(file.path().string(), status::created);
        }
        // if it is in our map
        else {
          // we update their last write times
          if (bucket[file.path().string()] !=
              current_file_last_write_time) {
            bucket[file.path().string()] =
                current_file_last_write_time;
            // and call the closure on them,
            // indicating modification
            action(file.path().string(), status::modified);
          }
        }
      }
    }
    // if this thing is a file
    else {
      // grab the last write time
      auto current_file_last_write_time =
          last_write_time(path_to_watch);
      // check if it's in our map
      if (!bucket.contains(path_to_watch)) {
        // put it there if not
        bucket[path_to_watch] =
            current_file_last_write_time;
        // and call the closure on it,
        // indicating creation
        action(path_to_watch, status::created);
      }
      // if it is in our map
      else {
        if (bucket[path_to_watch] !=
            current_file_last_write_time) {
          // I think you get the drift by now.
          bucket[path_to_watch] =
              current_file_last_write_time;
          action(path_to_watch, status::modified);
        }
      }
    }
  }

 private:
  std::unordered_map<std::string,
                     std::filesystem::file_time_type>
      bucket;
};

}  // namespace water
