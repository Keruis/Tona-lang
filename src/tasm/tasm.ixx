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
      /*
        section data:
          msg<byte>{"Hello, World[s:%d]\n", 0}
          len<imm>{std.sizeof(msg)}
        
        section text:
        main:
          reg.load r0, msg
          reg.load r1, len
          call printf(r0, r1)
          ret
      */
      std::vector<AsmToken> tokenize(std::string_view text) {
        const char* cur = text.data();
        
      }
    
  };

}