#include "lex/Lexer.h"

#include "basic/LexDiagnostic.h"

#include "basic/Source.h"

#include "basic/Token.h"

#include "support/UTF8.h"

#include "support/StringExtras.h"

#include "lex/LiteralSupport.h"

#include "support/UnicodeExtras.h"

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

void Lexer::AdvanceColumn(std::size_t length) noexcept {
  column_offset += length;
  last_was_cr = false;
}

void Lexer::Advance(char32_t cp, std::size_t length) noexcept {
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

void Lexer::UpdateToken(token::TokenClass cls, token::TokenType type, std::string_view text = {}) {
  tok.cls = cls;
  tok.type = type;
  tok.text = text;
  tok.location.length = cur - start;
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
  const bool equals_suffix = ((end - cur) >= 1) && cur[1] == '=';
  const std::size_t offset = 1 + equals_suffix;
  AdvanceCursor(offset);
  AdvanceColumn(offset);
  UpdateToken(
    token::TokenClass::OPERATOR, 
    static_cast<token::TokenType>(
      cp + 
      equals_suffix * token::equals_suffix_operator_offset
    )
  );
}

void Lexer::RepeatedCharOperator(char32_t cp) noexcept {
  char next_char = 0;
  if ((end - cur) >= 1)
    next_char = cur[1];
  const bool repeat = next_char == cp;
  const bool equals_suffix = next_char == '=';
  const std::size_t offset = 1 + (repeat || equals_suffix);
  AdvanceCursor(offset);
  AdvanceColumn(offset);
  UpdateToken(
    token::TokenClass::OPERATOR, 
    static_cast<token::TokenType>(
      cp + 
      repeat * token::repeated_char_operator_offset + 
      equals_suffix * token::equals_suffix_operator_offset
    )
  );
}

void Lexer::Punctuation(char32_t cp) noexcept {
  Advance(cp, 1);
  UpdateToken(
    token::TokenClass::PUNCTUATOR, 
    static_cast<token::TokenType>(cp)
  );
}

void Lexer::Identifier(char32_t cp, std::size_t length) {
  do {
    Advance(cp, length);
    cp = Peek(length);
  } while (cp == U'_' || unicode::IsXidContinue(cp));
  UpdateToken(
    token::TokenClass::IDENTIFIER, 
    token::TokenType::IDENTIFIER, 
    {start, cur}
  );
}

bool Lexer::RadixPrefix() noexcept {
  if (AtEnd())
    return false;

  switch (*cur) {  
    case 'b': case 'B':
      cur = ScanNumeric<Bin>(cur, end);
      break;
    case 'x': case 'X':
      cur = ScanNumeric<Hex>(cur, end);
      break;
    default:
      if (!IsDigit<Oct>(cur[-1]))
        return false;
      cur = ScanNumeric<Oct>(cur, end); 
  }
  return true;
}

void Lexer::FloatLiteral(token::TokenType& type) {
  if (AtEnd())
    return;
  
  switch (*cur) {
    case '.': 
      AdvanceCursor(1);
      cur = ScanNumeric<Dec>(cur, end);
      if (*cur == 'e' || *cur == 'E') {
    case 'e': case 'E':
        AdvanceCursor(1);
        cur += (*cur == '+' || *cur == '-');
        if (!IsDigit<Dec>(*cur)) [[unlikely]]
          Fail("");
        AdvanceCursor(1);
        cur = ScanNumeric<Dec>(cur, end);
      }
      type = token::TokenType::LITERALS_FLOAT;
    default:
  }
}

void Lexer::NumericSuffix(token::TokenType& type) {
  if (AtEnd())
    return;

  switch (*cur) {
    case 'u': case 'U':
    case 'i': case 'I':
      if (type == token::TokenType::LITERALS_FLOAT)
        Fail("");
    case 'f': case 'F':
      AdvanceCursor(1);
      type = static_cast<token::TokenType>(static_cast<std::uint8_t>(type) + 1);
      break;
    default: {
      std::size_t length;
      char32_t cp = Peek(length);
      if (cp == U'_' || unicode::IsXidStart(cp) || unicode::IsXidContinue(cp))
        Fail("Lexer::NumericLiteral");
      return;
    }
  }
  tok.suffix = 0;
  while (IsDigit<Dec>(*cur))
    tok.suffix = tok.suffix * 10 + (*cur - '0');
  switch (tok.suffix) {
    default:
      Fail("");
    case 8:
    case 16: 
    case 32: 
    case 64:
  }
}

void Lexer::NumericLiteral(char32_t cp) {
  token::TokenType type = token::TokenType::LITERALS_INT;
  AdvanceCursor(1);
  if (cp == U'0') {
    if (RadixPrefix())
      goto numeric_suf;
  } else cur = ScanNumeric<Dec>(cur, end);
  FloatLiteral(type);
numeric_suf:
  NumericSuffix(type);
  UpdateToken(
    token::TokenClass::LITERAL, 
    type,
    {start, cur - (tok.suffix ? tok.suffix >= 10 ? 3: 2: 0)}
  );
  AdvanceColumn(tok.location.length);
}

void Lexer::NextToken() {
  UpdatePosition();
  
  std::size_t length;
  char32_t cp = SkipTrivia(length);
  
  UpdatePosition();

  if (!length) {
    tok.location.length = 0;
    UpdateToken( 
      token::TokenClass::END,
      token::TokenType::END
    );
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