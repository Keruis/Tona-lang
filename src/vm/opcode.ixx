export module tona.opcode;

import std;

export namespace Tona {

  enum class OpCode : std::uint8_t {
    OC_SYS_EXIT,              // exit vm
    OC_SYS_ABORT,             // code = RA exit vm

    OC_DEBUG_PRINT_G,         // print(RA)
    OC_DEBUG_PRINT_F,         // print(FA)
    OC_DEBUG_PRINT_S,         // print(*RA)
    OC_DEBUG_PRINT_REGS,      // print(all regs)
    OC_DEBUG_PRINT_DUMPSTK,   // print(call frame)
    OC_DEBUG_PRINT_DUMPMEM,   // print(RA-RB hex dump mem)
    OC_DEBUG_ASSERT,          // if (RA == 0) sys.abort(AssertError)
    OC_DEBUG_BRK,             //
    OC_DEBUG_TICK,            // RA = time

    OC_STACK_ALLOC,           // sb += imm32
    OC_STACK_FREE,            // sb -= imm32
    OC_STACK_LOAD_8U,         // RA = stack[sb + imm32]
    OC_STACK_LOAD_8S,         // RA = stack[sb + imm32]
    OC_STACK_LOAD_16U,        // RA = stack[sb + imm32]
    OC_STACK_LOAD_16S,        // RA = stack[sb + imm32]
    OC_STACK_LOAD_32U,        // RA = stack[sb + imm32]
    OC_STACK_LOAD_32S,        // RA = stack[sb + imm32]
    OC_STACK_LOAD_64,         // RA = stack[sb + imm32]
    OC_STACK_LOAD_F32,        // RA = stack[sb + imm32]
    OC_STACK_LOAD_F64,        // RA = stack[sb + imm32]
    OC_STACK_STORE_8,         // stack[imm32] = RA
    OC_STACK_STORE_16,        // stack[imm32] = RA
    OC_STACK_STORE_32,        // stack[imm32] = RA
    OC_STACK_STORE_64,        // stack[imm32] = RA
    OC_STACK_STORE_F32,       // stack[imm32] = FA
    OC_STACK_STORE_F64,       // stack[imm32] = FA

    OC_MEMORY_ALLOC,          // RA = malloc(RB)
    OC_MEMORY_FREE,           // free(RA)
    OC_MEMORY_LOAD_8U,        // RA = memory[RB]
    OC_MEMORY_LOAD_8S,        // RA = memory[RB]
    OC_MEMORY_LOAD_16U,       // RA = memory[RB]
    OC_MEMORY_LOAD_16S,       // RA = memory[RB]
    OC_MEMORY_LOAD_32U,       // RA = memory[RB]
    OC_MEMORY_LOAD_32S,       // RA = memory[RB]
    OC_MEMORY_LOAD_64,        // RA = memory[RB]
    OC_MEMORY_LOAD_F32,       // RA = memory[RB]
    OC_MEMORY_LOAD_F64,       // RA = memory[RB]
    OC_MEMORY_STORE_8,        // memory[RA] = RB
    OC_MEMORY_STORE_16,       // memory[RA] = RB
    OC_MEMORY_STORE_32,       // memory[RA] = RB
    OC_MEMORY_STORE_64,       // memory[RA] = RB
    OC_MEMORY_STORE_F32,      // memory[RA] = FB
    OC_MEMORY_STORE_F64,      // memory[RA] = FB
    OC_MEMORY_CPY,            // memcpy(RA, RB, RC)
    OC_MEMORY_SET,            // memfill<width = imm32>(RA, RB, RC)
    OC_MEMORY_CMP,            // RA = memcmp(RB, RC, RD)

    OC_REG_MOVE,              // RA = RB
    OC_REG_MOVE_8S,           // RA = RB
    OC_REG_MOVE_16S,          // RA = RB
    OC_REG_MOVE_32S,          // RA = RB
    OC_REG_MOVE_F,            // FA = FB
    OC_REG_SWAP,              // swap(RA, RB)
    OC_REG_SWAP_F,            // swap(FA, FB)
    OC_REG_LOAD_8U,           // RA = imm8
    OC_REG_LOAD_8S,           // RA = imm8
    OC_REG_LOAD_16U,          // RA = imm16
    OC_REG_LOAD_16S,          // RA = imm16
    OC_REG_LOAD_32U,          // RA = imm32
    OC_REG_LOAD_32S,          // RA = imm32
    OC_REG_LOAD_64,           // RA = imm64
    OC_REG_LOAD_F32,          // FA = imm32
    OC_REG_LOAD_F64,          // FA = imm64

    OC_ALU_ITOF,              // FA = RB
    OC_ALU_UTOF,              // FA = RB
    OC_ALU_FTOI,              // RA = FB
    OC_ALU_FTOU,              // RA = FB
    OC_ALU_INC,               // RA = RA + 1
    OC_ALU_DEC,               // RA = RA - 1
    OC_ALU_NEG,               // RA = -RA
    OC_ALU_NEG_F,             // FA = -FA
    OC_ALU_ADD,               // RA = RB + RC
    OC_ALU_ADD_F,             // FA = FB + FC
    OC_ALU_SUB,               // RA = RB - RC
    OC_ALU_SUB_F,             // FA = FB - FC
    OC_ALU_MUL,               // RA = RB * RC
    OC_ALU_MUL_F,             // FA = FB * FC
    OC_ALU_DIV_U,             // RA = RB / RC
    OC_ALU_DIV_S,             // RA = RB / RC
    OC_ALU_DIV_F,             // FA = FB / FC
    OC_ALU_MOD_U,             // RA = RB % RC
    OC_ALU_MOD_S,             // RA = RB % RC
    OC_ALU_AND,               // RA = RB & RC
    OC_ALU_OR,                // RA = RB | RC
    OC_ALU_XOR,               // RA = RB ^ RC
    OC_ALU_NOT,               // RA = ~RB
    OC_ALU_SHL,               // RA = RB << RC
    OC_ALU_SHR,               // RA = RB >>(L) RC
    OC_ALU_SAR,               // RA = RB >>(A) RC


    OC_CTRL_JMP,              // ip += offset
    OC_CTRL_JMPO,             // ip += RA
    OC_CTRL_JE,               // if (RA == RB) ip += offset
    OC_CTRL_JNE,              // if (RA != RB) ip += offset
    OC_CTRL_JG,               // if (RA > RB) ip += offset
    OC_CTRL_JGE,              // if (RA >= RB) ip += offset
    OC_CTRL_JL,               // if (RA < RB) ip += offset
    OC_CTRL_JLE,              // if (RA <= RB) ip += offset
    OC_CTRL_JA,               // if (RA > RB) ip += offset
    OC_CTRL_JAE,              // if (RA >= RB) ip += offset
    OC_CTRL_JB,               // if (RA < RB) ip += offset
    OC_CTRL_JBE,              // if (RA <= RB) ip += offset
    OC_CTRL_FJE,              // if (FA == FB) ip += offset
    OC_CTRL_FJNE,             // if (FA != FB) ip += offset
    OC_CTRL_FJG,              // if (FA > FB) ip += offset
    OC_CTRL_FJGE,             // if (FA >= FB) ip += offset
    OC_CTRL_FJL,              // if (FA < FB) ip += offset
    OC_CTRL_FJLE,             // if (FA <= FB) ip += offset
    OC_CTRL_CALL,             // ip += offset base += shift
    OC_CTRL_RET,              // ip = ret_addr base = ret_base

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
    {"debug.printg", OpCode::OC_DEBUG_PRINT_G},
    {"debug.printf", OpCode::OC_DEBUG_PRINT_F},

    {"move", OpCode::OC_MOVE},
    {"fmove", OpCode::OC_FMOVE},
    {"load8", OpCode::OC_LOAD8},
    {"load16", OpCode::OC_LOAD16},
    {"load32", OpCode::OC_LOAD32},
    {"iload8", OpCode::OC_LOAD8S},
    {"iload16", OpCode::OC_LOAD16S},
    {"iload32", OpCode::OC_LOAD32S},
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

    {"fneg", OpCode::OC_FNEG},
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
    {"ftou", OpCode::OC_FTOU},

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

    {"sext8", OpCode::OC_SEXT8},
    {"sext16", OpCode::OC_SEXT16},
    {"sext32", OpCode::OC_SEXT32},

    {"call", OpCode::OC_CALL},
    {"ret", OpCode::OC_RET},

    {"ldm8", OpCode::OC_LDM8},
    {"ldm16", OpCode::OC_LDM16},
    {"ldm32", OpCode::OC_LDM32},
    {"ildm8", OpCode::OC_LDM8S},
    {"ildm16", OpCode::OC_LDM16S},
    {"ildm32", OpCode::OC_LDM32S},
    {"ldm", OpCode::OC_LDM},
    {"fldm32", OpCode::OC_FLDM32},
    {"fldm", OpCode::OC_FLDM},
    {"stm8", OpCode::OC_STM8},
    {"stm16", OpCode::OC_STM16},
    {"stm32", OpCode::OC_STM32},
    {"stm", OpCode::OC_STM},
    {"fstm32", OpCode::OC_FSTM32},
    {"fstm", OpCode::OC_FSTM},

    {"malloc", OpCode::OC_MALLOC},
    {"malloci", OpCode::OC_MALLOCI},
    {"free", OpCode::OC_FREE},
  };

  OpCode find_opcode(std::string_view text) {
    if (
      auto it = opcodes.find(text); 
      it != opcodes.end()
    ) [[likely]] return it->second;
    return OpCode::OC_UNKOWN;
  }
}