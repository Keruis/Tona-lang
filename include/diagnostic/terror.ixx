export module tona.error;

import std;

import tona.token;

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
    LET_INVALID_SUFFIX_TYPE,
    LET_INVALID_ESCAPE_SEQUENCE,
    LET_UNTERMINATED_STRING,
    LET_UNCLOSE_COMMENT,
    LET_OUT_OF_RANGE
  };

  struct LexError {
    std::string_view err_text;
    LexErrorType type;
    ErrorLevel lv;
  };

  // Parser
  enum class ParErrorType : std::uint8_t {
    PET_NONE,
    PET_DUPLICATE_TYPE_QUALIFIERS,
    PET_DUPLICATE_STORAGE_SPECIFIERS
  };

  struct ParError {
    Token tok;
    ParErrorType type;
    ErrorLevel lv;
  };

  // VM
  enum class VMErrorType : std::uint8_t {
    VMET_NONE,
    VMET_DOUBLE_FREE,
    VMET_CONSTANT_FREE,
    VMET_UNKNOW_FREE,
  };

}