module;

#include <cstdlib>

export module tona.diag;

import std;

import tona.error;
import tona.sourcefile;

export namespace Tona {

  class Diagnostic {
    public:
      [[noreturn]] void print_lex_err(std::size_t file_idx, const LexError& err, const SourceFiles& sf) {
        const auto ct = sf.find_char_table(file_idx, err.err_text.data());
        std::println("{}", (ct.line_text.data() == nullptr));
        std::cerr << std::format(
          "{}|{}:{}\n{}",
          sf.path(file_idx), ct.line, ct.col, ct.line_text
        ) << std::endl;
        std::exit(EXIT_FAILURE);
      }

    private:

  };

}