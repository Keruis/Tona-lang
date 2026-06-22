export module tona.assembler;

import std;

import tona.opcode;
import tona.byte;
import tona.arena;
import tona.asm_token;

export namespace Tona {

  class Assembler {
    public:
      Assembler(const Assembler&) = delete;
      Assembler& operator=(const Assembler&) = delete;
      Assembler(Assembler&&) = delete;
      Assembler& operator=(Assembler&&) = delete;

      Instruction* compile(std::string_view text) {

      }

    private:
      std::vector<AsmToken> tokenize() {

      }
    
  };

}