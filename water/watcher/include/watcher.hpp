#include <filesystem>
#include <functional>
#include <iostream>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <utility>

namespace water {

template <typename From>
concept String = requires(From str) {
  { str } -> std::convertible_to<std::string>;
};

template <typename From>
concept Path = String<From>;

template <typename Type, typename... Types>
concept Same = std::is_same<Type, Types...>::value;

template <typename... Types>
concept Invocable = requires(Types... values) {
  std::is_invocable<Types...>(values...) or
      std::is_invocable_v<Types...>(values...) or
      std::is_invocable_r_v<Types...>(values...);
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
 public:
  enum class status { created, modified, erased };

 private:
  std::unordered_map<std::string,
                     std::filesystem::file_time_type>
      bucket;
  std::string path;

 public:
  /** @brief watcher/constructor
   *  @param path - path to monitor for
   *  @see watcher::status
   *  Creates a file map from the
   *  given path.
   **/
  watcher(const Path auto path = ".")
      : path{std::string_view(path)} {
    using namespace std::filesystem;
    if (is_directory(path)) {
      for (const auto& file :
           recursive_directory_iterator(path)) {
        bucket[file.path().string()] =
            last_write_time(file);
      }
    } else {
      bucket[path] = last_write_time(path);
    }
  }

  auto get() {
    // refwrap the bucket as const
    // no funny business!
    using refwrap =
        std::reference_wrapper<const decltype(bucket)>;
    return refwrap(bucket);
  }

  void prune(const Callback auto callback) {
    using namespace std::filesystem;

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

  /** @brief watcher/run
   *  @param closure - action to perform on
   *  status "ticks"
   *  Monitors `path_to_watch` for changes.
   *  Executes the given closure when they
   *  happen.
   **/
  void run() {
    run([](std::string hit, status stat) {
      using ms = std::chrono::milliseconds;
      using std::cout;
      using std::endl;
      using std::this_thread::sleep_for;

      static constexpr auto delay = ms{16};

      switch (stat) {
        case status::created:
          cout << "created: " << hit << endl;
          break;
        case status::modified:
          cout << "modified: " << hit << endl;
          break;
        case status::erased:
          cout << "erased: " << hit << endl;
          break;
        default:
          cout << "unknown: " << hit << endl;
      }

      sleep_for(delay);
    });
  }

  void run(const Callback auto callback) {
    using namespace std::filesystem;

    prune(callback);

    // if this thing is a directory
    if (is_directory(path)) {
      // iterate through its contents
      for (const auto& file :
           recursive_directory_iterator(path)) {
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
            callback(file.path().string(),
                     status::modified);
          }
        }
      }
    }
    // if this thing is a file
    else if (is_regular_file(path)) {
      // grab the last write time
      const auto current_file_last_write_time =
          last_write_time(path);
      // check if it's in our map
      if (!bucket.contains(path)) {
        // put it there if not
        bucket[path] = current_file_last_write_time;
        // and call the closure on it,
        // indicating creation
        callback(path, status::created);
      }
      // if it is in our map
      else {
        if (bucket[path] != current_file_last_write_time) {
          // I think you get the drift by now.
          bucket[path] = current_file_last_write_time;
          callback(path, status::modified);
        }
      }
    }
  }
};

}  // namespace water
