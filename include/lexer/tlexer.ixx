export module tona.lexer;

import std;

import tona.buf;
import tona.diag;
import tona.token;
import tona.error;
import tona.arena;

export namespace Tona {

  class Lexer {
    public:
      Lexer(Diagnostic& diag)
        : diag(diag) 
      {}
      Lexer(const Lexer&) = delete;
      Lexer& operator=(const Lexer&) = delete;
      Lexer(Lexer&&) = delete;
      Lexer& operator=(Lexer&&) = delete;

      void tokenize(std::string_view text, std::pmr::vector<Token>& tokens, Arena& ca);

    private:
      [[nodiscard]] char parse_identifier(const char*& cur, std::pmr::vector<Token>& tokens);
      [[nodiscard]] bool parse_div(const char*& cur, std::pmr::vector<Token>& tokens);
      template <TokenClass cls>
      [[nodiscard]] char parse_single_char(const char*& cur, std::pmr::vector<Token>& tokens);
      [[nodiscard]] char parse_double_char(const char*& cur, std::pmr::vector<Token>& tokens);
      [[gnu::cold]] void invalid_char(const char* cur);
      template <typename... Args>
      [[gnu::cold]] void make_error(ErrorLevel lv, LexErrorType type, Args&&... args);

    private:
      void skip_line_comment(const char*& cur) noexcept;
      [[nodiscard]] bool skip_block_comment(const char*& cur) noexcept;
      template <auto PredFunc, typename FT = decltype(PredFunc)>
      void consume_digit_sequence(const char*& start) noexcept;
      template <auto PredFunc1, auto PredFunc2, typename FT1 = decltype(PredFunc1)> 
      [[nodiscard]] bool parse_radix_digits(const char*& start)
        noexcept requires(std::predicate<FT1, decltype(*start)>);
      [[nodiscard]] LexErrorType read_string(const char*& start, std::pmr::vector<Token>& tokens, Arena& arena);
      template <typename T, std::size_t Base = 10>
      [[nodiscard]] T scan_number(const char* start, const char* end);
    
    private:
      Diagnostic& diag;
      Buf buffer{};
  };

}