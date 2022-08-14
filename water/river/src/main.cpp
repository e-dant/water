#include <algorithm>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/config.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <ostream>
#include <river/config.hpp>
#include <river/fs.hpp>
#include <river/info.hpp>
#include <river/listener.hpp>
#include <river/session.hpp>
#include <string>
#include <thread>
#include <utility>
#include <vector>

auto main(int argc, char* argv[]) -> int try {
  constexpr const auto thread_count{7};
  const auto po{river::program_options(argc, argv)};
  auto ioc{boost::asio::io_context{thread_count}};
  auto thread_pool{std::map<int, std::thread>{}};
  auto listener{std::make_shared<river::listener>(
      ioc, boost::asio::ip::tcp::endpoint{po.address, po.port},
      std::make_shared<std::string>("./"))};

  listener->run();

  river::fs::rprint(".");
  info::soft("address:port(", po.address, ":", po.port, ")");

  for (auto i = 0; i < (thread_count - 1); i++) {
    thread_pool.insert(std::pair<int, std::thread>{i, [&ioc] { ioc.run(); }});
  }

  ioc.run();

  return 0;
} catch (std::exception const& e) {
  info::fatal("fatal standard exception", std::move(e.what()));
}
