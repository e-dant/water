#include <filesystem>
#include <functional>
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

namespace water {

template <typename From>
concept String = requires(From str) {
  { str } -> std::convertible_to<std::string>;
};

template <typename From>
concept Path = String<From>;

template<typename Type, typename... Types>
concept Same = std::is_same<Type, Types...>::value;

template<typename... Types>
concept Invocable = requires(Types... values) {
  std::is_invocable<Types...>(values...)
  or std::is_invocable_v<Types...>(values...)
  or std::is_invocable_r_v<Types...>(values...);
};

template <typename Returns, typename... Accepts>
concept Callback = requires(Returns fn, Accepts... args) {
  // A callback doesn't return a value,
  Same<void, Returns>
  // takes one or more arguments,
  and not Same<void, Accepts...>
  // and is callable.
  and Invocable<Returns, Accepts...>;
};

class watcher {
 private:
  std::unordered_map<std::string,
                     std::filesystem::file_time_type>
      bucket;
  std::string root;

 public:
  enum class status { created, modified, erased };

  /** @brief watcher/constructor
   *  @param path - path to monitor for
   *  @see watcher::status
   *  Creates a file map from the
   *  given path.
   **/
  watcher(const Path auto root = ".")
      : root{std::string_view(root)} {
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

  auto get() {
    using refwrap =
        std::reference_wrapper<const decltype(bucket)>;
    return refwrap(bucket);
  }

  /** @brief watcher/run
   *  @param closure - action to perform on
   *  status "ticks"
   *  Monitors `path_to_watch` for changes.
   *  Executes the given closure when they
   *  happen.
   **/
  void run(const Callback auto callback) {
    using namespace std::filesystem;

    {
      // first of all
      auto file = bucket.begin();
      while (file != bucket.end()) {
        // check if the stuff in our bucket
        // exists anymore
        if (!exists(file->first)) {
          // if not, call the closure on it,
          // indicating erasure
          callback(file->first, status::erased);
          // and get it out of here.
          // bucket, erase it!
          file = bucket.erase(file);
        } else {
          file++;
        }
      }
    }

    // if this thing is a directory
    if (is_directory(root)) {
      // iterate through its contents
      for (const auto& file :
           recursive_directory_iterator(root)) {
        if (is_regular_file(file)) {
          auto ec = std::error_code{};
          // grabbing the last write times
          const auto current_file_last_write_time =
              last_write_time(file, ec);
          // and checking for errors...
          if (ec) {
            // uh oh! the file disappeared while we
            // were (trying to) get a look at it.
            // it's gone, that's ok,
            // now let's call the closure
            callback(file.path().string(), status::erased);
            // and get it out of here
            bucket.erase(file.path().string());
          }
          // checking if they're in our map
          if (!bucket.contains(file.path().string())) {
            // putting them there if not
            bucket[file.path().string()] =
                current_file_last_write_time;
            // and calling the closure on them,
            // indicating creation
            callback(file.path().string(), status::created);
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
              callback(file.path().string(), status::modified);
            }
          }
        }
      }
    }
    // if this thing is a file
    else if(is_regular_file(root)) {
      // grab the last write time
      const auto current_file_last_write_time =
          last_write_time(root);
      // check if it's in our map
      if (!bucket.contains(root)) {
        // put it there if not
        bucket[root] = current_file_last_write_time;
        // and call the closure on it,
        // indicating creation
        callback(root, status::created);
      }
      // if it is in our map
      else {
        if (bucket[root] != current_file_last_write_time) {
          // I think you get the drift by now.
          bucket[root] = current_file_last_write_time;
          callback(root, status::modified);
        }
      }
    }
  }
};

}  // namespace water
