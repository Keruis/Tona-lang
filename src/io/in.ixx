export module tona.in;

import std;

export namespace Tona {

  class FileReader {
    public:
      std::expected<std::string, std::error_code> read_file(const std::filesystem::path& path) {
        std::error_code ec{};
        if (!std::filesystem::exists(path, ec) || !std::filesystem::is_regular_file(path, ec))
          return std::unexpected(ec); 

        std::ifstream file(path);
        if (!file)
          return std::unexpected(std::make_error_code(std::errc::io_error));

        std::uintmax_t size;
        if (std::filesystem::file_size(path, ec) == static_cast<std::uintmax_t>(-1))
          return std::unexpected(ec);

        std::string buffer;
        buffer.resize(size + 16);

        if (!file.read(&buffer[0], size))
          return std::unexpected(std::make_error_code(std::errc::io_error));

        return buffer;
      }
  };

}