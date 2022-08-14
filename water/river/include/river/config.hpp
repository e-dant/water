#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <boost/program_options.hpp>
#include <cassert>
#include <cstdlib>
#include <iostream>

namespace river {

namespace po = boost::program_options;
namespace net = boost::asio;

struct program_options {
  net::ip::address address;
  unsigned short port;

  /** @function Config
   * parses and initializes `port` and `address` from the given program
   * arguments. prints `opt_desc` and exits the program if the `help` option is
   * given. */
  program_options(int& argc, char** argv) {
    po::options_description arg_desc("river");
    arg_desc.add_options()(

        "help,h", "print this help message")(

        "address,a", po::value<std::string>()->default_value("0.0.0.0"),
        "set listening address")(

        "port,p", po::value<std::uint16_t>()->default_value(9080),
        "set listening port");
    po::variables_map arg_map;

    po::store(po::parse_command_line(argc, argv, arg_desc), arg_map);
    po::notify(arg_map);
    if (arg_map.count("help")) {
      std::cout << arg_desc << "\n";
      std::exit(EXIT_SUCCESS);
    }

    this->port = arg_map["port"].as<std::uint16_t>();
    this->address =
        boost::asio::ip::make_address(arg_map["address"].as<std::string>());
  };

};  // struct

}  // namespace river
