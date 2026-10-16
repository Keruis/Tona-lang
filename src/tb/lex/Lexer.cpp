#include "lex/Lexer.h"

#include "basic/LexDiagnostic.h"

#include "basic/Source.h"

#include "basic/Token.h"

#include "support/UTF8.h"

#include "support/StringExtras.h"

#include "lex/LiteralSupport.h"

#include "support/UnicodeExtras.h"

#include "utility/Algorithms.h"

#include <cstring>

namespace tb {

[[noreturn]] void Lexer::Fail(std::string message) const {
  Diagnostic diag{
    .message = std::move(message),
    .line = tok.location.line,
    .column = tok.location.column,
    .source_line = source_line({reinterpret_cast<const char*>(source.data()), source.size()}, tok.location.line)
  };
  throw LexError(std::move(diag));
}

bool Lexer::AtEnd() const noexcept {
  return cur >= end;
}

char32_t Lexer::PeekAt(std::size_t offset, std::size_t& length) const {
  char32_t cp;
  auto pos = cur;
  while(pos < end) {
    if (!DecodeAt(pos, end, cp, length))
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
    ? 1 + column_offset
    : tok.location.column + column_offset;
  line_offset = 0;
  column_offset = 0;
}

void Lexer::UpdateToken(TokenClass cls, TokenType type, std::string_view text = {}) {
  tok.cls = cls;
  tok.type = type;
  tok.text = text;
  tok.location.length = cur - start;
}

char32_t Lexer::SkipTrivia(std::size_t& length) {
  while (!AtEnd()) {
    char32_t cp = Peek(length);
    if (IsUnicodeWhitespace(cp) || IsUnicodeLineTerminator(cp)) {
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
      while (!AtEnd() && !IsUnicodeLineTerminator(cp = Peek(length)))
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
        Fail("Lexer::SkipTrivia");
      UpdatePosition();
      continue;
    }
    return cp;
  }
  UpdatePosition();
  return length = 0, U'\0';
}

void Lexer::EqualsSuffixOperator(char32_t cp) noexcept {
  const bool equals_suffix = ((end - cur) >= 2) && cur[1] == '=';
  const std::size_t offset = 1 + equals_suffix;
  AdvanceCursor(offset);
  AdvanceColumn(offset);
  UpdateToken(
    TokenClass::OPERATOR, 
    static_cast<TokenType>(
      cp + 
      equals_suffix * equals_suffix_operator_offset
    )
  );
}

void Lexer::RepeatedCharOperator(char32_t cp) noexcept {
  char next_char = 0;
  if ((end - cur) >= 2)
    next_char = cur[1];
  const bool repeat = next_char == cp;
  const bool equals_suffix = next_char == '=';
  const std::size_t offset = 1 + (repeat || equals_suffix);
  AdvanceCursor(offset);
  AdvanceColumn(offset);
  UpdateToken(
    TokenClass::OPERATOR, 
    static_cast<TokenType>(
      cp + 
      repeat * repeated_char_operator_offset + 
      equals_suffix * equals_suffix_operator_offset
    )
  );
}

void Lexer::Punctuation(char32_t cp) noexcept {
  Advance(cp, 1);
  UpdateToken(
    TokenClass::PUNCTUATOR, 
    static_cast<TokenType>(cp)
  );
}

void Lexer::Identifier(char32_t cp, std::size_t length) {
  do {
    Advance(cp, length);
    cp = Peek(length);
  } while (cp == U'_' || IsXidStart(cp) || IsXidContinue(cp));
  std::string_view text{start, cur};
  auto type = FindKeyword(text);
  UpdateToken(
    type != TokenType::IDENTIFIER ? TokenClass::KEYWORD : TokenClass::IDENTIFIER, 
    type, 
    text
  );
}

bool Lexer::RadixPrefix() noexcept {
  if (AtEnd())
    return false;

  switch (*cur) {  
    case 'b': case 'B':
      AdvanceCursor(1);
      cur = ScanNumeric<Bin>(cur, end);
      goto fail;
    case 'x': case 'X':
      AdvanceCursor(1);
      cur = ScanNumeric<Hex>(cur, end);
fail:
      if (cur - start <= 2)
        Fail("Lexer::RadixPrefix");
      break;
    default:
      if (!IsDigit<Oct>(*cur))
        return false;
      AdvanceCursor(1);
      cur = ScanNumeric<Oct>(cur, end); 
  }
  return true;
}

void Lexer::FloatLiteral(TokenType& type) {
  if (AtEnd())
    return;
  
  switch (*cur) {
    case '.': 
      AdvanceCursor(1);
      cur = ScanNumeric<Dec>(cur, end);
      if (!AtEnd() && (*cur == 'e' || *cur == 'E')) {
    case 'e': case 'E':
        AdvanceCursor(1);
        if (!AtEnd()) {
          cur += (*cur == '+' || *cur == '-');
          if (!IsDigit<Dec>(*cur)) [[unlikely]]
            Fail("");
          AdvanceCursor(1);
          cur = ScanNumeric<Dec>(cur, end);
        }
      }
      type = TokenType::LITERALS_FLOAT;
    default:
  }
}

void Lexer::NumericSuffix(TokenType& type) {
  char32_t cp;
  std::size_t length;
  if (AtEnd())
    return;

  switch (*cur) {
    case 'u': case 'U':
    case 'i': case 'I':
      if (type == TokenType::LITERALS_FLOAT)
        Fail("Lexer::NumericSuffix_1");
      AdvanceCursor(1);
      type = TokenType::LITERALS_INT_SUF;
    case 'f': case 'F':
      AdvanceCursor(1);
      type = TokenType::LITERALS_FLOAT_SUF;
      break;
    default: goto fail;
  }

  tok.suffix = 0;
  while (!AtEnd() && IsDigit<Dec>(*cur))
    tok.suffix = tok.suffix * 10 + (*cur++ - '0');
  if (!utility::IsAnyOf(tok.suffix, 8, 16, 32, 64)) 
    Fail("Lexer::NumericSuffix_2");
fail:
  cp = Peek(length);
  if (cp == U'_' || IsXidStart(cp) || IsXidContinue(cp))
    Fail("Lexer::NumericLiteral_3");
}

void Lexer::NumericLiteral(char32_t cp) {
  TokenType type = TokenType::LITERALS_INT;
  AdvanceCursor(1);
  if (cp == U'0') {
    if (RadixPrefix())
      goto numeric_suf;
  } else cur = ScanNumeric<Dec>(cur, end);
  FloatLiteral(type);
numeric_suf:
  NumericSuffix(type);
  UpdateToken(
    TokenClass::LITERAL, 
    type,
    {start, cur - (tok.suffix ? tok.suffix >= 10 ? 3: 2: 0)}
  );
  AdvanceColumn(tok.location.length);
}

void Lexer::Escape(char32_t cp) noexcept {
  auto& character = tok.character;
  switch (cp) {
    case U'n': character = '\n'; break;
    case U'r': character = '\r'; break;
    case U't': character = '\t'; break;
    case U'v': character = '\v'; break;
    case U'a': character = '\a'; break;
    case U'b': character = '\b'; break;
    default:
      character = cp;
  }
}

void Lexer::StringLiteral() {
  auto& character = tok.character;
  auto& str = tok.str;
  TokenType type = TokenType::LITERALS_STRING_VIEW;
  char32_t cp;
  std::size_t length;
  Advance(U'"', 1);
  while (!AtEnd()) {
    cp = Peek(length);
    if (IsUnicodeLineTerminator(cp)) [[unlikely]]
      Fail("Lexer::StringLiteral_1");
    if (cp == U'"') {
      Advance(cp, length);
      UpdateToken(
        TokenClass::LITERAL,
        type
      );
      if (type == TokenType::LITERALS_STRING_VIEW)
        tok.text = {start + 1, cur - 1};
      return;
    }
    if (cp == U'\\') {
      if (type == TokenType::LITERALS_STRING_VIEW)
        str.assign(start + 1, cur - 1);
      type = TokenType::LITERALS_STRING;
      Advance(cp, length);
      cp = Peek(length);
      Escape(cp);
      str.push_back(character);
    }
    Advance(cp, length);
  }
  Fail("Lexer::StringLiteral_2");
}

void Lexer::CharacterLiteral() {
  char32_t cp;
  std::size_t length;
  Advance(U'\'', 1);
  cp = Peek(length);
  if (IsUnicodeLineTerminator(cp))
    Fail("Lexer::CharacterLiteral_1");
  if (cp == U'\'')
    Fail("Lexer::CharacterLiteral_2");
  if (cp == U'\\') {
    Advance(cp, length);
    cp = Peek(length);
    Escape(cp);
  } 
  if (length != 1)
    Fail("Lexer::CharacterLiteral_3");
  Advance(cp, length);
  cp = Peek(length);
  if (cp != '\'')
    Fail("Lexer::CharacterLiteral_4");
  UpdateToken(
    TokenClass::LITERAL, 
    TokenType::LITERALS_CHARACTER
  );
}

TokenType Lexer::FindKeyword(std::string_view text) noexcept {
  if (text == "if")
    return TokenType::KEYWORD_IF;
  if (text == "else")
    return TokenType::KEYWORD_ELSE;
  if (text == "true")
    return TokenType::KEYWORD_TRUE;
  if (text == "false")
    return TokenType::KEYWORD_FALSE;
  return TokenType::IDENTIFIER;
}

void Lexer::NextToken() {
  UpdatePosition();
  
  std::size_t length;
  char32_t cp = SkipTrivia(length);
  
  UpdatePosition();

  if (!length) {
    UpdateToken( 
      TokenClass::END,
      TokenType::END
    );
    tok.location.length = 0;
    return;
  }

  start = cur;
  switch (cp) {
    case U'0' ... U'9':
      return NumericLiteral(cp);

    case U'"':
      return StringLiteral();

    case U'\'':
      return CharacterLiteral();

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
      if (cp == '_' || IsXidStart(cp))
        return Identifier(cp, length);
  }
  Fail("Lexer::NextToken");
}

}