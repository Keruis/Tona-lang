#include "tb.h"

#include <print>

int main() {
  std::string_view text = R"(
    11.2
  )";
  tb::Lexer lex(text);
  lex.NextToken();
  auto& tok = lex.RefToken();
  std::println("{}", tok.text);
}