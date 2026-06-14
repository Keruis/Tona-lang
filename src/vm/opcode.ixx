export module tona.opcode;

import std;

export namespace Tona {

  enum class OpCode : std::uint8_t {
    OC_END,
    OC_PRINT_G, // print(RA)
    OC_PRINT_F, // print(FA)
    OC_MOVE, // RA = RB
    OC_FMOVE, // FA = FB
    OC_LOAD8, // RA = Imm8
    OC_LOAD16, // RA = Imm8
    OC_LOAD32, // RA = Imm32
    OC_LOAD, // RA = Imm64
    OC_FLOAD32, // FA = F32
    OC_FLOAD, // FA = F64

    OC_ADD, // RA = RB + RC
    OC_SUB, // RA = RB - RC
    OC_MUL, // RA = RB * RC
    OC_DIV, // RA = RB / RC
    OC_MOD, // RA = RB % RC

    OC_FADD, // FA = FB + FC
    OC_FSUB, // FA = FB - FC
    OC_FMUL, // FA = FB * FC
    OC_FDIV, // FA = FB / FC

    OC_ITOF, // FB = RA
    OC_FTOI, // RB = FA

    OC_JMP, // ip += offset
    OC_JEQ, // if (RA == RB) ip += offset
    OC_JNE, // if (RA != RB) ip += offset
    OC_JLT, // if (RA < RB) ip += offset
    OC_JLE, // if (RA <= RB) ip += offset
    OC_JGT, // if (RA > RB) ip += offset
    OC_JGE, // if (RA >= RB) ip += offset
    OC_FJEQ, // if (FA == FB) ip += offset
    OC_FJNE, // if (FA != FB) ip += offset
    OC_FJLT, // if (FA < FB) ip += offset
    OC_FJLE, // if (FA <= FB) ip += offset
    OC_FJGT, // if (FA > FB) ip += offset
    OC_FJGE, // if (FA >= FB) ip += offset

    OC_CALL, // ip += offset base += shift
    OC_RET // ip = ret_addr base = ret_base
  };

  using GPRegister = std::uint64_t;
  using FPRegister = double;

  using Instruction = std::uint8_t;

  struct CallFrame {
    const Instruction* ret_addr;
    std::size_t base;
  };

  const std::flat_map<std::string_view, OpCode> opcodes {
    {"end", OpCode::OC_END},
    {"printg", OpCode::OC_PRINT_G},
    {"printf", OpCode::OC_PRINT_F},
    {"move", OpCode::OC_MOVE},
    {"fmove", OpCode::OC_FMOVE},
    {"load8", OpCode::OC_LOAD8},
    {"load16", OpCode::OC_LOAD16},
    {"load32", OpCode::OC_LOAD32},
    {"load", OpCode::OC_LOAD},
    {"fload32", OpCode::OC_FLOAD32},
    {"fload", OpCode::OC_FLOAD},

    {"add", OpCode::OC_ADD},
    {"sub", OpCode::OC_SUB},
    {"mul", OpCode::OC_MUL},
    {"div", OpCode::OC_DIV},
    {"mod", OpCode::OC_MOD},
    {"fadd", OpCode::OC_FADD},
    {"fsub", OpCode::OC_FSUB},
    {"fmul", OpCode::OC_FMUL},
    {"fdiv", OpCode::OC_FDIV},

    {"itof", OpCode::OC_ITOF},
    {"ftoi", OpCode::OC_FTOI},

    {"jmp", OpCode::OC_JMP},
    {"jeq", OpCode::OC_JEQ},
    {"jne", OpCode::OC_JNE},
    {"jlt", OpCode::OC_JLT},
    {"jle", OpCode::OC_JLE},
    {"jgt", OpCode::OC_JGT},
    {"jge", OpCode::OC_JGE},
    {"fjeq", OpCode::OC_FJEQ},
    {"fjne", OpCode::OC_FJNE},
    {"fjlt", OpCode::OC_FJLT},
    {"fjle", OpCode::OC_FJLE},
    {"fjgt", OpCode::OC_FJGT},
    {"fjge", OpCode::OC_FJGE},

    {"call", OpCode::OC_CALL},
    {"ret", OpCode::OC_RET}
  };

  OpCode find_opcode(std::string_view text) {
    if (
      auto it = opcodes.find(text); 
      it != opcodes.end()
    ) return it->second;
    return OpCode::OC_END;
  }
}