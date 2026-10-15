#ifndef __LEXER__H__
#define __LEXER__H__

#include "../basic/Token.h"

#include <span>
#include <cstddef>

namespace tb {

class Lexer {
  public:
    Lexer(std::span<const char> source)
      : source(source), cur(source.data()), end(source.data() + source.size()) 
    {}

    void NextToken();

  private:
    [[noreturn]] void Fail(std::string) const;

    bool AtEnd() const noexcept;
    char32_t PeekAt(std::size_t, std::size_t&) const;
    char32_t Peek(std::size_t&) const;
    char32_t PeekNext(std::size_t&) const;
    void AdvanceCursor(std::size_t) noexcept;
    void AdvanceColumn(std::size_t) noexcept;
    void Advance(char32_t, std::size_t) noexcept;
    void UpdatePosition() noexcept;
    void UpdateToken(TokenClass, TokenType, std::string_view);
    char32_t SkipTrivia(std::size_t&);

  private:
    void EqualsSuffixOperator(char32_t) noexcept;
    void RepeatedCharOperator(char32_t) noexcept;
    void Punctuation(char32_t) noexcept;
    void Identifier(char32_t, std::size_t);
    bool RadixPrefix() noexcept;
    void FloatLiteral(TokenType&);
    void NumericSuffix(TokenType&);
    void NumericLiteral(char32_t);

  private:
    std::span<const char> source;
    Token tok{};

    const char* cur = nullptr;
    const char* end = nullptr;

    const char* start = nullptr;
    std::size_t column_offset = 0;
    std::size_t line_offset = 0;
    bool last_was_cr = false;
};

}

#endif