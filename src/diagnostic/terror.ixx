export module tona.error;

import std;

export namespace Tona {

  enum class ErrorLevel : std::uint8_t {
    EL_WARNING,
    EL_ERROR
  };

  // Lexer
  enum class LexErrorType : std::uint8_t {
    LET_NONE,
    LET_INVALID_CHAR,
    LET_INVALID_DIGIT_SEPARATOR,
    LET_INVALID_NUMERIC_LITERAL,
    LET_UNTERMINATED_STRING,
    LET_UNCLOSE_COMMENT
  };

  struct LexError {
    std::string_view err_text;
    LexErrorType type;
  };

  // VM
  enum class VMErrorType : std::uint8_t {
    VMET_NONE,
  };

  // Heap
  enum class HeapErrorType : std::uint8_t {
    HET_NONE,
    HET_DOUBLE_FREE,
    HET_CONSTANT_FREE,
    HET_UNKNOW_FREE,
  };

}