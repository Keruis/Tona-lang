export module tona.elysia;

import std;

import tona.lexer;
import tona.token;
import tona.diag;
import tona.arena;
import tona.error;
import tona.sourcefile;

export namespace Tona {

  class Elysia {
    public:
      Elysia(std::ostream& out_stream)
        : out(out_stream) {}

      int do_file(std::string_view file_path) {
        SourceFiles sf;
        Diagnostic diag;
        Lexer lex(diag);
        std::size_t file_idx = sf.registry_file(file_path);
        Arena arena(4096);
        Arena const_arena(4096);
        std::pmr::vector<Token> tokens(&arena);
        auto start = std::chrono::high_resolution_clock::now();
        lex.tokenize(sf.text(file_idx), tokens, const_arena);
        auto end = std::chrono::high_resolution_clock::now();
        std::println("time: {}", std::chrono::duration_cast<std::chrono::microseconds>(end - start));
        //if (res.type != LexErrorType::LET_NONE)
          //diag.print_lex_err(file_idx, res, sf);
        print_token(tokens);
        return 0;
      }

    private:
      void print_token(std::span<Token> tokens) {
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

        for (const auto& tok : tokens) {
          std::string value_str;
          switch (tok.cls) {
            case TokenClass::C_END:
            case TokenClass::C_IDENTIFIER:
              value_str = std::string_view(tok.text.data, tok.text.len);
              break;
            case TokenClass::C_LITERAL:
              if (tok.type == TokenType::T_LITERALS_STRING)
                value_str = std::format("\"{}\"", std::string_view(tok.text.data, tok.text.len));
              else {
                switch (tok.type) {
                  case TokenType::T_LITERALS_INT:
                  case TokenType::T_LITERALS_INT_SUF:
                    value_str = std::format("i: {:#b} suf: {}", tok.num.val, tok.num.suf);
                    break;
                  case TokenType::T_LITERALS_FLOAT:
                  case TokenType::T_LITERALS_FLOAT_SUF:
                    value_str = std::format("f: {:#b} suf: {}", tok.num.val, tok.num.suf);
                }
              }
              break;
            case TokenClass::C_KEYWORD:
              value_str = std::format("kw_id({})", static_cast<int>(tok.type));
              break;
            case TokenClass::C_OPERATOR:
            case TokenClass::C_PUNCTUATOR:
              value_str = std::format("{}", static_cast<char>(tok.type));
          }

          out << std::format("│ {:<12} │ {:<41} │\n", get_class_name(tok.cls), value_str);
        }

        out << "└──────────────​┴───────────────────────────────────────────┘\n\n";
      }

    private:
      std::ostream& out;
  };

}