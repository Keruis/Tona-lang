module;

#include <cstdlib>

export module tona.diag;

import std;

import tona.error;
import tona.sourcefile;

export namespace Tona {

  class Diagnostic {
    public:
      template <typename... Args>
        requires (std::constructible_from<std::string_view, Args...>)
      void push_lex_err(ErrorLevel lv, LexErrorType type, Args... args) {
        lex_errs.push_back({
          .err_text = std::string_view(std::forward<Args>(args)...),
          .type = type,
          .lv = lv
        });
      }

      [[noreturn]] void print_lex_err(std::size_t file_idx, const LexError& err, const SourceFiles& sf) {
        const auto ct = sf.find_char_table(file_idx, err.err_text.data());
        std::cerr << std::format(
          "{}|{}:{}\n{}",
          sf.path(file_idx), ct.line, ct.col, ct.line_text
        ) << std::endl;
        std::exit(EXIT_FAILURE);
      }

    private:
      std::vector<LexError> lex_errs;
  };

}