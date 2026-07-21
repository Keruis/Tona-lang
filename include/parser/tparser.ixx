export module tona.parser;

import std;

import tona.byte;
import tona.type;
import tona.diag;
import tona.token;
import tona.error;
import tona.bitflag;

export namespace Tona {

  class Parser {
    public:
      Parser(Diagnostic& diag) 
        : diag(diag) 
      {}
      Parser(const Parser&) = delete;
      Parser& operator=(const Parser&) = delete;
      Parser(Parser&&) = delete;
      Parser& operator=(Parser&&) = delete;

      void parse(std::span<const Token> tokens);

    private:
      void parse_keyword(const Token*& tokens);
      void parse_identifier(const Token*& tokens);

    private:
      [[nodiscard]] StorageSpecifiers parse_storage_specifiers(const Token*& tokens);
      [[nodiscard]] TypeQualifiers parse_type_qualifiers(const Token*& tokens);

    private:


    private:
      Diagnostic& diag;
  };

}