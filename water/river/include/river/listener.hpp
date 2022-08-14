#pragma once

#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <river/info.hpp>
#include <river/session.hpp>

// Accepts incoming connections and launches the sessions
namespace river {
//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

class listener : public std::enable_shared_from_this<listener> {
  boost::asio::io_context& ioc_;
  boost::asio::ip::tcp::acceptor acceptor_;
  std::shared_ptr<std::string const> doc_root_;

 public:
  listener(boost::asio::io_context& ioc,
           boost::asio::ip::tcp::endpoint endpoint,
           std::shared_ptr<std::string const> const& doc_root)
      : ioc_(ioc),
        acceptor_(boost::asio::make_strand(ioc)),
        doc_root_(doc_root) {
    boost::beast::error_code e;

    acceptor_.open(endpoint.protocol(), e);
    if (e) info::fatal("listener error opening socket: ", e.what());

    acceptor_.set_option(boost::asio::socket_base::reuse_address(true), e);
    if (e) info::fatal("listener error reusing address: ", e.what());

    acceptor_.bind(endpoint, e);
    if (e) info::fatal("listener error binding endpoint: ", e.what());

    acceptor_.listen(boost::asio::socket_base::max_listen_connections, e);
    if (e) info::fatal("listener error: ", e.what());
  }

  // Start accepting incoming connections
  void run() { do_accept(); }

 private:
  void do_accept() {
    // The new connection gets its own strand
    acceptor_.async_accept(boost::asio::make_strand(ioc_),
                           boost::beast::bind_front_handler(
                               &listener::on_accept, shared_from_this()));
  }

  void on_accept(boost::beast::error_code e,
                 boost::asio::ip::tcp::socket socket) {
    if (e) {
      info::soft("listener error: ", e.what());
      return;  // To avoid infinite loop
    } else {
      // Create the session and run it
      std::make_shared<session>(std::move(socket), doc_root_)->run();
    }

    // Accept another connection
    do_accept();
  }
};
}  // namespace river
