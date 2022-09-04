#include <chrono>
#include <filesystem>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>

namespace water {

namespace concepts {
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

}  // namespace concepts

namespace watcher {

using namespace concepts;

enum class status { created, modified, erased };

// anonymous namespace
// for "private" variables
// (via internal linkage)
namespace {

// shorthands for
//  - directory_options
//  - follow_directory_symlink
//  - skip_permission_denied
//  - bucket_t, a map of strings and
//  times
using dir_opt_t = std::filesystem::directory_options;
using std::filesystem::directory_options::
    follow_directory_symlink;
using std::filesystem::directory_options::
    skip_permission_denied;
using bucket_t
    = std::unordered_map<std::string,
                         std::filesystem::file_time_type>;

// we need these variables to make
// caching state easier
// ... which is what objects are for,
// right? well, maybe this should be an
// object, I'm not sure, I just like
// functions is all.
static constexpr dir_opt_t dir_opt  // NOLINT
    = skip_permission_denied & follow_directory_symlink;

static bucket_t bucket;  // NOLINT
}  // namespace

/* @brief watcher/constructor
 * @param path - path to monitor for
 * @see watcher::status
 * Creates a file map from the
 * given path. */
void populate(const Path auto path = {"."}) {
  using namespace std::filesystem;
  using dir_iter  = recursive_directory_iterator;

  auto good_count = 1;
  auto bad_count  = 1;

  if (exists(path)) {
    if (is_directory(path)) {
      try {
        for (const auto& file : dir_iter(path, dir_opt)) {
          if (exists(file)) {
            good_count++;
            bucket[file.path().string()]
                = last_write_time(file);
          } else {
            bad_count++;
          }
        }
      } catch (const std::exception& e) {
        bad_count++;
      }
    } else {
      good_count++;
      bucket[path] = last_write_time(path);
    }
  } else {
    throw std::runtime_error{"path does not exist."};
  }

  std::cout << "watching " << good_count << " files."
            << std::endl;
  if (bad_count > 1)
    std::cout << "skipped " << bad_count << " files."
              << std::endl;
  else if (bad_count == 0)
    std::cout << "skipped 1 file." << std::endl;
}

/* @brief prune
 * Removes non-existent files
 * from our bucket. */
auto prune(const Path auto path,
           const Callback auto callback) {
  using std::filesystem::exists;

  // first of all
  auto file = bucket.begin();
  file == bucket.end()
      // if the beginning is the end,
      // try to populate the files
      ? populate(path)
      // otherwise, iterate over the
      // bucket's contents
      : [&]() {
          while (file != bucket.end()) {
            // check if the stuff in our bucket
            // exists anymore
            exists(file->first)
                // if it does,
                // move on to the next file
                ? std::advance(file, 1)
                // otherwise, call the closue on it,
                // indicating erasure,
                // and remove it from our bucket.
                : [&]() {
                    callback(file->first, status::erased);
                    // bucket, erase it!
                    file = bucket.erase(file);
                  }();
          }
        }();
}

/* @brief scan_file
 * Scans a single file.
 * Updates the bucket.
 * Calls the callback. */
bool scan_file(const Path auto file,
               const Callback auto callback) {
  using namespace std::filesystem;
  if (is_regular_file(file)) {
    auto ec              = std::error_code{};
    // grabbing the last write times
    const auto timestamp = last_write_time(file, ec);
    // and checking for errors...
    if (ec) {
      // uh oh! the file disappeared
      // while we were (trying to) get a
      // look at it. it's gone, that's
      // ok, now let's call the closure,
      // indicating erasure,
      callback(file, status::erased);
      // and get it out of the bucket.
      if (bucket.contains(file))
        bucket.erase(file);
    }
    // checking if they're in our map
    if (!bucket.contains(file)) {
      // putting them there if not
      bucket[file] = timestamp;
      // and calling the closure on
      // them, indicating creation
      callback(file, status::created);
    }
    // if it is in our map
    else {
      // we update their last write
      // times
      if (bucket[file] != timestamp) {
        bucket[file] = timestamp;
        // and call the closure on them,
        // indicating modification
        callback(file, status::modified);
      }
    }
    return true;
  } else
    return false;
}

bool scan_directory(const Path auto dir,
                    const Callback auto callback) {
  using namespace std::filesystem;
  using dir_iter = recursive_directory_iterator;

  // if this thing is a directory
  if (is_directory(dir)) {
    // trying to iterate through its
    // contents
    try {
      for (const auto& file : dir_iter(dir, dir_opt)) {
        scan_file(file.path(), callback);
      }
    }  // and catching the error(s?)
    catch (const std::exception& e) {
      // @todo
      // figure out how to grab the file
      // the directory iterator was
      // looking at and remove it from
      // our bucket. maybe putting this
      // try `block` inside of a `while`
      // loop (instead of above a
      // `for`).
      //
      // uh oh! the file disappeared
      // while we were (trying to) get a
      // look at it. it's gone, that's
      // ok, now let's call the closure,
      // indicating erasure,
      //
      // callback(file.path(),
      // status::erased);
      //// and get it out of the bucket.
      // if
      // (bucket.contains(file.path()))
      //   bucket.erase(file.path());
    }
    return true;
  } else
    return false;
}

/* @briref watcher/scan
 * if this `path` is a directory,
 * scan recursively through its
 * contents. otherwise, this `path` is a
 * file,å so scan it alone. */
bool scan(const Path auto path,
          const Callback auto callback) {
  using namespace std::filesystem;
  // keep ourselves clean
  prune(path, callback);
  // clang-format off
  // and scan, if the path exists.
  return exists(path)
         ? scan_directory(path, callback)
           ? true
           : scan_file(path, callback)
             ? true
             : false
         : false;
  // clang-format on
}

/* @brief watcher/run
 * @param closure (optional):
 *  A callback to perform when the files
 *  being watched change.
 *  @see Callback
 * Monitors `path_to_watch` for changes.
 * Executes the given closure when they
 * happen. */
template <const auto delay_ms = 16>
bool run(const Path auto path,
         const Callback auto callback) requires
    std::is_integral_v<decltype(delay_ms)> {
  using std::this_thread::sleep_for,
      std::chrono::milliseconds;

  while (scan(path, callback))
    if constexpr (delay_ms > 0)
      sleep_for(milliseconds(delay_ms));

  return false;

  /* @note
  alternatively,
  we could use this syntax:

    return scan(path, callback)
               // if no errors present,
               // keep running
               ? run(path, callback)
               // otherwise, leave
               : false;

  which may or may not be more clear.
  i don't know.
  */
}

}  // namespace watcher
}  // namespace water
