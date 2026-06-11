module;

#include <cassert>

export module tona.sourcefile;

import std;

import tona.in;
import tona.io_err;

export namespace Tona {

  struct CharTable {
    std::string_view line_text;
    std::size_t col;
    std::size_t line;
  };

  class SourceFiles {
    public:
      std::size_t registry_file(std::string_view path) {
        FileReader fr;

        auto res = fr.read_file(path);
        if (!res.has_value()) [[unlikely]]
          io_err(path, res.error());
        
        path_files.push_back(path);
        text_files.push_back(std::move(res.value()));

        auto& text = text_files.back();
        std::vector<const char*> vec_lines;
        vec_lines.reserve(text.size() / 40);
        vec_lines.push_back(text.data());

        const char* start = text.data();
        const char* end = text.data() + text.size();
        const char* cur = text.data();

        while (cur < end) {
          const char* ptr = static_cast<const char*>(std::memchr(cur, '\n', end - cur));
          if (!ptr) [[unlikely]]
            break;
          cur = ptr + 1;
          vec_lines.push_back(cur);
        }

        line_starts.push_back(std::move(vec_lines));

        return path_files.size() - 1;
      }

      std::string_view path(std::size_t file_idx) const {
        assert(file_idx >= path_files.size());
        return path_files[file_idx];
      }

      std::string_view text(std::size_t file_idx) const {
        assert(file_idx >= path_files.size());
        return text_files[file_idx];
      }

      CharTable find_char_table(std::size_t file_idx, const char* ptr) const {
        const auto& vec_lines = line_starts[file_idx];

        CharTable ct;
        std::size_t idx = get_range(file_idx, ptr);
        
        const char* const start = vec_lines[idx];
        const char* const end = vec_lines[idx + 1];

        ct.col = ptr - start;
        ct.line = idx;
        
        std::size_t len = end - start;
        if (len > 0 && start[len - 1] == '\n') len--;
        if (len > 0 && start[len - 1] == '\r') len--;
        ct.line_text = std::string_view(start, len);

        return ct;
      }

    private:
      std::size_t get_range(std::size_t file_idx, const char* ptr) const {
        const auto& vec_lines = line_starts[file_idx];
        auto it = std::upper_bound(vec_lines.begin(), vec_lines.end(), ptr);
        return std::distance(vec_lines.begin(), it) - 1;
      }

    private:
      std::vector<std::string> text_files;
      std::vector<std::string_view> path_files;
      std::vector<std::vector<const char*>> line_starts;
  };

}