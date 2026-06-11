export module tona.elysia;

import std;

import tona.lexer;
import tona.token;
import tona.sourcefile;
import tona.diag;

export namespace Tona {

  class Elysia {
    public:
      Elysia(std::ostream& out_stream)
        : out(out_stream) {}

      int do_file(std::string_view file_path) {
        SourceFiles sf;
        Diagnostic diag;
        Lexer lex{};
        std::size_t file_idx = sf.registry_file(file_path);
        auto token_ctx = lex.tokenize(sf.text(file_idx));
        if (!token_ctx.has_value())
          diag.print_lex_err(file_idx, token_ctx.error(), sf);
        print_token(token_ctx.value());
        return 0;
      }

    private:
      void print_token(const TokenContext& ctx) {
        auto get_class_name = [](TokenClass cls) -> std::string_view {
          switch (cls) {
            case TokenClass::C_KEYWORD:    return "KEYWORD";
            case TokenClass::C_IDENTIFIER: return "IDENTIFIER";
            case TokenClass::C_LITERAL:    return "LITERAL";
            case TokenClass::C_OPERATOR:   return "OPERATOR";
            case TokenClass::C_PUNCTUATOR: return "PUNCTUATOR";
            case TokenClass::C_END:        return "END";
          }
        };

        out << "\n┌──────────────┬───── Tona Token Stream ───────────────────┐\n";
        out << std::format("│ {:<12} │ {:<41} │\n", "TOKEN CLASS", "VALUE");
        out << "├──────────────​┼───────────────────────────────────────────┤\n";

        for (const auto& tok : ctx.tokens) {
          std::string value_str;
          auto cls = tokens_info[static_cast<std::size_t>(tok.type)].cls;
          switch (cls) {
            case TokenClass::C_END:
            case TokenClass::C_IDENTIFIER:
            case TokenClass::C_LITERAL:
              if (tok.type == TokenType::T_LITERALS_STRING)
                value_str = std::format("\"{}\"", ctx.strings[tok.str_idx]);
              else
                value_str = std::string_view(tok.text.data, tok.text.len);
              break;
            case TokenClass::C_KEYWORD:
              value_str = std::format("kw_id({})", static_cast<int>(tok.type));
              break;
            case TokenClass::C_OPERATOR:
            case TokenClass::C_PUNCTUATOR:
              value_str = std::format("{}", static_cast<char>(tok.type));
          }

          out << std::format("│ {:<12} │ {:<41} │\n", get_class_name(cls), value_str);
        }

        out << "└──────────────​┴───────────────────────────────────────────┘\n\n";
      }

    private:
      std::ostream& out;
  };

}