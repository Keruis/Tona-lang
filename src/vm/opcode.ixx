export module tona.opcode;

import std;

export namespace Tona {

  enum class OpCode : std::uint8_t {
    OC_MOVE, // RA = RB
    OC_FMOVE, // FA = FB
    OC_LOAD, // RA = Imm
    OC_FLOAD, // RA = F

    OC_ADD, // RA = RB + RC
    OC_SUB, // RA = RB - RC
    OC_MUL, // RA = RB * RC
    OC_DIV, // RA = RB / RC
    OC_MOD, // RA = RB % RC

    OC_FADD, // FA = FB + FC
    OC_FSUB, // FA = FB - FC
    OC_FMUL, // FA = FB * FC
    OC_FDIV, // FA = FB / FC

  };

  using GPRegister = std::uint64_t;
  using FPRegister = double;

  using Instruction = OpCode;

}