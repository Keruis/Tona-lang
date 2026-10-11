#include "../../../include/tb/lex/Lexer.h"

#include "../../../include/tb/basic/LexDiagnostic.h"

#include "../../../include/tb/basic/Source.h"

#include "../../../include/tb/support/UTF8.h"

#include "../../../include/tb/support/StringExtras.h"

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
  return cur >= &source.back();
}

char32_t Lexer::Peek() const noexcept {
  if (AtEnd())
    return U'\0';
  char32_t cp;
  std::size_t length;
  if (!unicode::DecodeAt(cur, &source.back(), cp, length))
    Fail("invail utf8 character");
  return cp;
}

void Lexer::NextToken() {
  if (AtEnd()) {
    tok.type = token::TokenType::END;
    tok.cls = token::TokenClass::END;
    tok.location.length = 0;
    return;
  }

  auto cp = Peek();

  if (IsAscii(cp))
  ;

  switch (cp) {
    default: {
      if (cp == U'_' || unicode::IsXidStart(cp)) {

      }
    }
  }
}

}