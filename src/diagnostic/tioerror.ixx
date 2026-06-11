module;

#include <cstdlib>

export module tona.io_err;

import std;

export namespace Tona {

  [[noreturn]] void io_err(const std::filesystem::path& path, std::error_code code) {
    if (code == std::errc::no_such_file_or_directory) {
      std::cerr << std::format(
        "error no_such_file file_name: {}",
        path.string()
      ) << std::endl;
    }
    if (code == std::errc::permission_denied) {
      std::cerr << std::format(
        "error permission_denied file_name: {}",
        path.string()
      ) << std::endl;
    }

    std::cerr << "unkown error" << std::endl;

    std::exit(EXIT_FAILURE);
  }

}