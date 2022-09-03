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
  static constexpr auto dir_opts =
      std::filesystem::directory_options::
          skip_permission_denied &
      std::filesystem::directory_options::
          follow_directory_symlink;
  std::unordered_map<std::string,
                     std::filesystem::file_time_type>
      bucket;
  std::string path;

 public:
  /* @brief watcher/constructor
   * @param path - path to monitor for
   * @see watcher::status
   * Creates a file map from the
   * given path. */
  watcher(const Path auto path = ".")
      : path{std::string_view(path)} {
    using namespace std::filesystem;
    using dir_iter = recursive_directory_iterator;

    if (exists(path)) {
      if (is_directory(path)) {
        for (const auto& file : dir_iter(path))
          if (exists(file))
            bucket[file.path().string()] =
                last_write_time(file);
      } else {
        bucket[path] = last_write_time(path);
      }
    } else {
      throw std::runtime_error{"path does not exist."};
    }
  }

  /* @brief get
   * Returns a const (wrapped) reference to our
   * (private) bucket. */
  auto get() {
    // reference wrap the bucket as const --
    // no funny business!
    using ref_wrap =
        std::reference_wrapper<const decltype(bucket)>;
    return ref_wrap(bucket);
  }

  /* @brief prune
   * Removes non-existent files
   * from our bucket. */
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

  /* @brief scan_file
   * Scans a single file.
   * Updates the bucket.
   * Calls the callback. */
  void scan_file(const Path auto file,
                 const Callback auto callback) {
    using namespace std::filesystem;
    if (exists(file) && is_regular_file(file)) {
      auto ec_read = std::error_code{};
      // grabbing the last write times
      const auto current_file_last_write_time =
          last_write_time(file, ec_read);
      // and checking for errors...
      if (ec_read) {
        // uh oh! the file disappeared while we
        // were (trying to) get a look at it.
        // it's gone, that's ok,
        // now let's call the closure,
        // indicating erasure,
        callback(file, status::erased);
        // and get it out of the bucket.
        if (bucket.contains(file))
          bucket.erase(file);
      }
      // checking if they're in our map
      if (!bucket.contains(file)) {
        // putting them there if not
        bucket[file] = current_file_last_write_time;
        // and calling the closure on them,
        // indicating creation
        callback(file, status::created);
      }
      // if it is in our map
      else {
        // we update their last write times
        if (bucket[file] != current_file_last_write_time) {
          bucket[file] = current_file_last_write_time;
          // and call the closure on them,
          // indicating modification
          callback(file, status::modified);
        }
      }
    }
  }

  /* @brief watcher/run
   * @param closure (optional):
   *  A callback to perform when the files
   *  being watched change.
   *  @see Callback
   * Monitors `path_to_watch` for changes.
   * Executes the given closure when they
   * happen. */
  void run(const Callback auto callback) {
    using namespace std::filesystem;
    using dir_iter = recursive_directory_iterator;

    // keep ourselves clean
    prune(callback);

    if (exists(path)) {
      // if this thing is a directory
      if (is_directory(path))
        // iterate through its contents
        for (const auto& file : dir_iter(path, dir_opts)) {
          std::cout << "scanning " << file.path()
                    << std::endl;
          // scanning all the while
          scan_file(file.path(), callback);
        }
      // if this thing is a file
      else if (is_regular_file(path))
        // scan it alone
        scan_file(path, callback);
    }
  }

  void run() {
    std::cout << "watching " << path << std::endl;
    run([](const Path auto f, status s) {
      static constexpr auto delay =
          std::chrono::milliseconds{16};
      switch (s) {
        case status::created:
          std::cout << "created: " << f << std::endl;
          break;
        case status::modified:
          std::cout << "modified: " << f << std::endl;
          break;
        case status::erased:
          std::cout << "erased: " << f << std::endl;
          break;
        default:
          std::cout << "unknown: " << f << std::endl;
      }
      std::this_thread::sleep_for(delay);
    });
  }
};

}  // namespace water
