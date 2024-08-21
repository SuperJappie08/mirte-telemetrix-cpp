#pragma once

#include <boost/lexical_cast.hpp>
#include <optional>
#include <string>
#include <vector>
inline std::optional<int> try_parse_int(std::string str) {
  try {
    return boost::lexical_cast<int>(str);
  } catch (...) {
    return {};
  }
}
inline std::optional<double> try_parse_double(std::string str) {
  try {
    return boost::lexical_cast<double>(str);
  } catch (...) {
    return {};
  }
}
inline bool starts_with(std::string str, std::string prefix) {
  return str.rfind(prefix, 0) == 0;
}

#include <array>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

std::string exec(const char *cmd);

#include <filesystem>
#include <iostream>
#include <string>

namespace fs = std::filesystem;
struct serial_port {
  std::string port_name;
  uint pid;
  uint vid;
};
std::vector<serial_port> get_available_ports();