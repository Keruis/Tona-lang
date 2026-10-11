#ifndef __LEXER__H__
#define __LEXER__H__

#include "../basic/Token.h"
#include "../basic/LexDiagnostic.h"

#include <span>

namespace tb {

class Lexer {
  public:
    Lexer(std::span<const char> source)
      : source(source), cur(source.data()) {}

    void NextToken();

  private:
    [[noreturn]] void Fail(std::string) const;

    bool AtEnd() const noexcept;
    char32_t Peek() const noexcept;

  private:
    std::span<const char> source;
    token::Token tok{};

    const char* cur = nullptr;
};

}

#endif