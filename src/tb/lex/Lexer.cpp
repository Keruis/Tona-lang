#include "lex/Lexer.h"

#include "basic/LexDiagnostic.h"

#include "basic/Source.h"

#include "support/UTF8.h"

#include "support/StringExtras.h"

namespace tb {

[[noreturn]] void Lexer::Fail(std::string message) const {
  err::Diagnostic diag{
    .message = std::move(message),
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
  }
  return length = 0, U'\0';
}

char32_t Lexer::Peek(std::size_t& length) const {
  return PeekAt(0, length);
}

char32_t Lexer::PeekNext(std::size_t& length) const {
  return PeekAt(1, length);
}

void Lexer::AdvanceCursor(std::size_t length) noexcept {
  cur += length;
}

void Lexer::Advance(char32_t cp, std::size_t length) noexcept {
  auto& line = tok.location.line;
  auto& column = tok.location.column;
  AdvanceCursor(length);
  switch (cp) {
    case U'\n':
      if (last_was_cr)
        break;
    case U'\r':
    case U'\u2028':
    case U'\u2029':
    case U'\u0085':
      line_offset++;
      column_offset = 0;
      break;
    default:
      column_offset++;
  }
  last_was_cr = cp == U'\r';
}

void Lexer::UpdatePosition() noexcept {
  tok.location.line += line_offset;
  tok.location.column = line_offset 
    ? tok.location.column 
    : tok.location.column + column_offset;
  line_offset = 0;
  column_offset = 0;
}

char32_t Lexer::SkipTrivia(std::size_t& length) {
  while (!AtEnd()) {
    char32_t cp = Peek(length);
    if (unicode::IsUnicodeWhitespace(cp) || unicode::IsUnicodeLineTerminator(cp)) {
      Advance(cp, length);
      continue;
    }
    if (cp != U'/')
      return cp;
    std::size_t length_next;
    auto cp_next = PeekNext(length_next);
    if (cp_next == U'/') {
      Advance(U'/', 1);
      Advance(U'/', 1);
      while (!AtEnd() && !unicode::IsUnicodeLineTerminator(cp = Peek(length)))
        Advance(cp, length);
      UpdatePosition();
      continue;
    }
    if (cp_next == U'*') {
      bool closed = false;
      Advance(U'/', 1);
      Advance(U'*', 1);
      while (!AtEnd()) {
        if ((cp = Peek(length)) == U'*' && (cp_next = PeekNext(length_next)) == U'/') {
          Advance(U'*', 1);
          Advance(U'/', 1);
          closed = true;
          break;
        }
        Advance(cp, length);
      }
      if (!closed) [[unlikely]] 
        Fail("unterminated block comment");
      UpdatePosition();
      continue;
    }
    return cp;
  }
  UpdatePosition();
  return length = 0, U'\0';
}

void Lexer::EqualsSuffixOperator(char32_t cp) noexcept {
  bool equals_suffix = ((end - cur) >= 1) && cur[1] == '=';
  std::size_t offset = 1 + equals_suffix;
  AdvanceCursor(offset);
  column_offset += offset;
  tok.cls = token::TokenClass::OPERATOR;
  tok.type = static_cast<token::TokenType>(
    cp + 
    equals_suffix * token::equals_suffix_operator_offset
  );
  tok.location.length = cur - start;
}

void Lexer::RepeatedCharOperator(char32_t cp) noexcept {
  const char next_char = (end - cur) >= 1 && cur[1];
  bool repeat = next_char == cp;
  bool equals_suffix = next_char == '=';
  std::size_t offset = 1 + (repeat || equals_suffix);
  AdvanceCursor(offset);
  column_offset += offset;
  tok.cls = token::TokenClass::OPERATOR;
  tok.type = static_cast<token::TokenType>(
    cp + 
    repeat * token::repeated_char_operator_offset + 
    equals_suffix * token::equals_suffix_operator_offset
  );
  tok.location.length = cur - start;
}

void Lexer::Punctuation(char32_t cp) noexcept {
  Advance(cp, 1);
  tok.cls = token::TokenClass::PUNCTUATOR;
  tok.type = static_cast<token::TokenType>(cp);
  tok.location.length = cur - start;
}

void Lexer::Identifier(char32_t cp, std::size_t length) noexcept {
  std::size_t consumed_length = length;
  do {
    Advance(cp, length);
    cp = Peek(length);
  } while ((cp == U'_' || unicode::IsXidContinue(cp)) && (consumed_length += length));
  tok.cls = token::TokenClass::IDENTIFIER;
  tok.type = token::TokenType::IDENTIFIER;
  tok.text = {start, consumed_length};
  tok.location.length = cur - start;
}

void Lexer::NumericLiteral(char32_t cp) {
  
}

void Lexer::NextToken() {
  UpdatePosition();
  
  std::size_t length;
  char32_t cp = SkipTrivia(length);
  if (!length) {
    tok.type = token::TokenType::END;
    tok.cls = token::TokenClass::END;
    tok.text = {};
    tok.location.length = 0;
    return;
  }

  start = cur;
  switch (cp) {
    case U'0' ... U'9':
      return NumericLiteral(cp);
    
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
      return Punctuation(cp);
    
    default:
      if (cp == '_' || unicode::IsXidStart(cp))
        return Identifier(cp, length);
  }
  Fail("Lexer::NextToken");
}

}