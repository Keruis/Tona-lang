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
        section data
          msg<byte>{"Hello, World[s:%d]\n", 0}
          len<imm>{sizeof(msg)}
        
        section text
        main:
          reg.load8u r0, msg
          reg.load8u r1, len
          call printf(r0, r1)
          ret
      */
      std::vector<AsmToken> tokenize(std::string_view text) {
        std::vector<AsmToken> vec_tokens;

        const char* cur = text.data();
        
        while (*cur) {
          switch (*cur) {
            case 'a'...'z':
            case 'A'...'Z':
            case '_': {
              const char* const end_ptr = identifier_char(cur);
              std::string_view identifier(cur, end_ptr);

              if (auto res = find_opcode(identifier); res != OpCode::OC_UNKOWN)
                vec_tokens.push_back({
                  .type = AsmTokenType::ATT_OPCODE,
                  .op = res
                });
              else vec_tokens.push_back({
                .type = AsmTokenType::ATT_IDENTIFIER,
                .text = identifier
              });
              cur = end_ptr;
              break;
            }

            case ':': {
              vec_tokens.push_back({
                .type = AsmTokenType::ATT_PUNCTUATORS_COLON
              });
              cur++;
              break;
            }

            case '0'...'9': {
              const char* const start_ptr = cur;
              do {
                cur++;
              } while (is_dec_char(*cur));
              
              
              break;
            }

            case ',': {

            }

            case '"':
          }

        }

        return vec_tokens;
      }
  };

}