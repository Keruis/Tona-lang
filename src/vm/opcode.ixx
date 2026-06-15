export module tona.opcode;

import std;

export namespace Tona {

  enum class OpCode : std::uint8_t {
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

    OC_INC, // RA = RB + 1
    OC_DEC, // RA = RB - 1
    OC_NEG, // RA = -RB
    OC_ADD, // RA = RB + RC
    OC_SUB, // RA = RB - RC
    OC_MUL, // RA = RB * RC
    OC_DIV, // RA = RB / RC
    OC_DIVS, // RA = RB / RC
    OC_MOD, // RA = RB % RC
    OC_MODS, // RA = RB % RC

    OC_FNEG, // FA = -FB
    OC_FADD, // FA = FB + FC
    OC_FSUB, // FA = FB - FC
    OC_FMUL, // FA = FB * FC
    OC_FDIV, // FA = FB / FC
    OC_FMAX, // FA = max(FB, FC)
    OC_FMIN, // FA = min(FB, FC)
    OC_FSQRT, // FA = ----
    //              _/ FB

    OC_ITOF, // FB = RA
    OC_UTOF, // FB = RA
    OC_FTOI, // RB = FA

    OC_JMP, // ip += offset
    OC_JMPO, // ip += RA
    OC_JE, // if (RA == RB) ip += offset
    OC_JNE, // if (RA != RB) ip += offset
    OC_JG, // if (RA > RB) ip += offset
    OC_JGE, // if (RA >= RB) ip += offset
    OC_JL, // if (RA < RB) ip += offset
    OC_JLE, // if (RA <= RB) ip += offset
    OC_JA, // if (RA > RB) ip += offset
    OC_JAE, // if (RA >= RB) ip += offset
    OC_JB, // if (RA < RB) ip += offset
    OC_JBE, // if (RA <= RB) ip += offset
    OC_FJE, // if (FA == FB) ip += offset
    OC_FJNE, // if (FA != FB) ip += offset
    OC_FJG, // if (FA > FB) ip += offset
    OC_FJGE, // if (FA >= FB) ip += offset
    OC_FJL, // if (FA < FB) ip += offset
    OC_FJLE, // if (FA <= FB) ip += offset

    OC_AND, // RC = RA & RB
    OC_OR, // RC = RA | RB
    OC_XOR, // RC = RA ^ RB
    OC_NOT, // RB = ~RA

    OC_SHL, // RC = RA << RB
    OC_SHR, // RC = RA >>(L) RB
    OC_SAR, // RC = RA >>(A) RB

    OC_CALL, // ip += offset base += shift
    OC_RET, // ip = ret_addr base = ret_base

    OC_LDM8, // RA = mem[RB]
    OC_LDM16, // RA = mem[RB]
    OC_LDM32, // RA = mem[RB]
    OC_LDM, // RA = mem[RB]
    OC_FLDM32, // FA = mem[RB]
    OC_FLDM, // FA = mem[RB]
    OC_STM8, // mem[RB] = RA
    OC_STM16, // mem[RB] = RA
    OC_STM32, // mem[RB] = RA
    OC_STM, // mem[RB] = RA
    OC_FSTM32, // mem[RB] = FA
    OC_FSTM, // mem[RB] = FA

    //OC_GROW, // RA = malloc(RB)
    OC_UNKOWN = std::numeric_limits<std::uint8_t>::max(),
  };

  using GPRegister = std::uint64_t;
  using FPRegister = double;

  using Instruction = std::uint8_t;

  struct CallFrame {
    const Instruction* ret_addr;
    std::size_t base;
  };

  const std::flat_map<std::string_view, OpCode> opcodes {
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

    {"inc", OpCode::OC_INC},
    {"dec", OpCode::OC_DEC},
    {"neg", OpCode::OC_NEG},
    {"add", OpCode::OC_ADD},
    {"sub", OpCode::OC_SUB},
    {"mul", OpCode::OC_MUL},
    {"div", OpCode::OC_DIV},
    {"idiv", OpCode::OC_DIVS},
    {"mod", OpCode::OC_MOD},
    {"imod", OpCode::OC_MODS},

    {"fadd", OpCode::OC_FADD},
    {"fsub", OpCode::OC_FSUB},
    {"fmul", OpCode::OC_FMUL},
    {"fdiv", OpCode::OC_FDIV},
    {"fmax", OpCode::OC_FMAX},
    {"fmin", OpCode::OC_FMIN},
    {"fsqrt", OpCode::OC_FSQRT},

    {"itof", OpCode::OC_ITOF},
    {"utof", OpCode::OC_UTOF},
    {"ftoi", OpCode::OC_FTOI},

    {"jmp", OpCode::OC_JMP},
    {"jmpo", OpCode::OC_JMPO},
    {"je", OpCode::OC_JE},
    {"jne", OpCode::OC_JNE},
    {"jg", OpCode::OC_JG},
    {"jge", OpCode::OC_JGE},
    {"jl", OpCode::OC_JL},
    {"jle", OpCode::OC_JLE},
    {"ja", OpCode::OC_JA},
    {"jae", OpCode::OC_JAE},
    {"jb", OpCode::OC_JB},
    {"jbe", OpCode::OC_JBE},
    {"fje", OpCode::OC_FJE},
    {"fjne", OpCode::OC_FJNE},
    {"fjg", OpCode::OC_FJG},
    {"fjge", OpCode::OC_FJGE},
    {"fjl", OpCode::OC_FJL},
    {"fjle", OpCode::OC_FJLE},

    {"and", OpCode::OC_AND},
    {"or", OpCode::OC_OR},
    {"xor", OpCode::OC_XOR},
    {"not", OpCode::OC_NOT},

    {"shl", OpCode::OC_SHL},
    {"shr", OpCode::OC_SHR},
    {"sar", OpCode::OC_SAR},

    {"call", OpCode::OC_CALL},
    {"ret", OpCode::OC_RET},

    {"ldm8", OpCode::OC_LDM8},
    {"ldm16", OpCode::OC_LDM16},
    {"ldm32", OpCode::OC_LDM32},
    {"ldm", OpCode::OC_LDM},
    {"fldm32", OpCode::OC_FLDM32},
    {"fldm", OpCode::OC_FLDM},
    {"stm8", OpCode::OC_STM8},
    {"stm16", OpCode::OC_STM16},
    {"stm32", OpCode::OC_STM32},
    {"stm", OpCode::OC_STM},
    {"fstm32", OpCode::OC_FSTM32},
    {"fstm", OpCode::OC_FSTM},

    //{"grow", OpCode::OC_GROW}
  };

  OpCode find_opcode(std::string_view text) {
    if (
      auto it = opcodes.find(text); 
      it != opcodes.end()
    ) [[likely]] return it->second;
    return OpCode::OC_UNKOWN;
  }
}