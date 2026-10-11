#include "../../../include/tb/lex/Lexer.h"

#include "../../../include/tb/basic/LexDiagnostic.h"

#include "../../../include/tb/basic/Source.h"

#include "../../../include/tb/support/UTF8.h"

#include "../../../include/tb/support/StringExtras.h"

#include <cstddef>

namespace tb {

[[noreturn]] void Lexer::Fail(std::string message) const {
  err::Diagnostic diag{
    .message = message,
    .line = tok.location.line,
    .column = tok.location.column,
    .source_line = source_line({reinterpret_cast<const char*>(source.data()), source.size()}, tok.location.line)
  };
  throw err::LexError(std::move(diag));
}

bool Lexer::AtEnd() const noexcept {
  return cur >= end;
}

char32_t Lexer::PeekAt(std::size_t offset, std::size_t& length) const {
  char32_t cp;
  auto pos = cur;
  while(pos != end) {
    if (!unicode::DecodeAt(pos, end, cp, length))
      Fail("Lexer::PeekAt");
    if (!offset)
      return cp;
    pos += length;
    offset--; 
  };
  return length = 0, U'\0';
}

char32_t Lexer::Peek(std::size_t& length) const {
  return PeekAt(0, length);
}

char32_t Lexer::PeekNext(std::size_t& length) const {
  return PeekAt(1, length);
}

void Lexer::Advance(char32_t cp, std::size_t length) noexcept {
  auto& line = tok.location.line;
  auto& column = tok.location.column;
  static bool last_was_cr = false;
  cur += length;
  switch (cp) {
    case U'\n':
      if (last_was_cr)
        break;
    case U'\r':
    case U'\u2028':
    case U'\u2029':
    case U'\u0085':
      line++;
      column = 1;
      break;
    default:
      column++;
  }
  last_was_cr = cp == U'\r';
}

char32_t Lexer::SkipTrivia(std::size_t& length) {
  char32_t cp = U'\0';
  while (!AtEnd()) {
    cp = Peek(length);
    if (unicode::IsUnicodeWhitespace(cp) || unicode::IsUnicodeLineTerminator(cp)) {
      Advance(cp, length);
      continue;
    }
    if (cp != U'/')
      break;
    std::size_t length_next;
    auto cp_next = PeekNext(length_next);
    if (cp_next == U'/') {
      Advance(cp, length);
      Advance(cp_next, length_next);
      while (!AtEnd() && !unicode::IsUnicodeLineTerminator(cp = Peek(length)))
        Advance(cp, length);
      continue;
    }
    if (cp_next == U'*') {
      bool closed = false;
      const std::size_t comment_line = tok.location.line;
      const std::size_t comment_column = tok.location.column;
      Advance(cp, length);
      Advance(cp_next, length_next);
      while (!AtEnd()) {
        if ((cp = Peek(length)) == U'*' && (cp_next = PeekNext(length_next)) == U'/') {
          Advance(cp, length);
          Advance(cp_next, length_next);
          closed = true;
          break;
        }
        Advance(cp, length);
      }
      if (!closed) [[unlikely]] {
        tok.location.line = comment_line;
        tok.location.column = comment_column;
        Fail("unterminated block comment");
      }
      continue;
    }
    return cp;
  }
  length = 0;
  return U'\0';
}

void Lexer::EqualsSuffixOperator(char32_t cp) noexcept {
  const auto start = cur;
  bool equals_suffix = (end - cur) >= 1 && cur[1] == '=';
  Advance(cp, 1 + equals_suffix);
  tok.cls = token::TokenClass::OPERATOR;
  tok.type = static_cast<token::TokenType>(
    cp + 
    equals_suffix * token::equals_suffix_operator_offset
  );
  tok.location.length = cur - start;
}

void Lexer::RepeatedCharOperator(char32_t cp) noexcept {
  const auto start = cur;
  const bool has_remaining = (end - cur) >= 1;
  bool repeat = has_remaining && cur[1] == cp;
  bool equals_suffix = has_remaining && cur[1] == '=';
  Advance(cp, 1 + (repeat || equals_suffix));
  tok.cls = token::TokenClass::OPERATOR;
  tok.type = static_cast<token::TokenType>(
    cp + 
    repeat * token::repeated_char_operator_offset + 
    equals_suffix * token::equals_suffix_operator_offset
  );
  tok.location.length = cur - start;
}

void Lexer::NextToken() {
  char32_t cp;
  std::size_t length;
  if (!(cp = SkipTrivia(length))) {
    tok.type = token::TokenType::END;
    tok.cls = token::TokenClass::END;
    tok.text = {};
    tok.location.length = 0;
    return;
  }
  
  switch (cp) {
    case U'0' ... U'9':
    
    case U'"':

    case U'\'':

    case U'!': case U'=':
    case U'*': case U'/':
    case U'%':
      return EqualsSuffixOperator(cp);

    case U'>': case U'<': 
    case U'+': case U'-':
      return RepeatedCharOperator(cp);

    case U'(': case U')':
    case U'{': case U'}':
    case U'[': case U']':
    case U',': case U':':
    case U';':
    
    default:
  }

}

}