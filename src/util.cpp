#include "util.hpp"
std::string exec(const char *cmd) {
  std::array<char, 128> buffer;
  std::string result;
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
  if (!pipe) {
    throw std::runtime_error("popen() failed!");
  }
  while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) !=
         nullptr) {
    result += buffer.data();
  }
  return result;
}

std::vector<serial_port> get_available_ports() {
  std::vector<serial_port> port_names;

  fs::path p("/dev/serial/by-id");
  try {
    if (!exists(p)) {
      std::cout << p.generic_string() << " does not exist" << std::endl;
      return port_names;
    } else {
      for (auto de : fs::directory_iterator(p)) {
        if (is_symlink(de.symlink_status())) {
          serial_port sp;
          fs::path symlink_points_at = read_symlink(de);
          fs::path canonical_path = fs::canonical(p / symlink_points_at);
          std::cout << canonical_path.generic_string() << std::endl;
          sp.port_name = canonical_path.generic_string();
          // sp.pid = ;
          sp.pid = std::stoi(exec(("udevadm info --name=" + canonical_path.generic_string() +
                         " --attribute-walk | grep idProduct")
                            .c_str()), nullptr, 16);
          sp.vid = std::stoi(exec(("udevadm info --name=" + canonical_path.generic_string() +
                          " --attribute-walk | grep idVendor")
                              .c_str()), nullptr, 16);
          std::cout << sp.port_name << " " << sp.vid << " " << sp.pid << std::endl;
          port_names.push_back(sp);
        }
      }
    }
  } catch (const fs::filesystem_error &ex) {
    std::cout << ex.what() << '\n';
    throw port_names;
  }
  // std::sort(port_names.begin(), port_names.end());
  return port_names;
}
