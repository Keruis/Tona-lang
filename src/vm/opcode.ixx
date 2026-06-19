export module tona.opcode;

import std;

export namespace Tona {

  enum class OpCode : std::uint8_t {
    OC_SYS_EXIT,              // exit vm
    OC_SYS_ABORT,             // code = R exit vm

    OC_DEBUG_PRINT_G,         // print(A)
    OC_DEBUG_PRINT_F,         // print(A)
    OC_DEBUG_PRINT_S,         // print(*A)
    OC_DEBUG_PRINT_REGS,      // print(all regs)
    OC_DEBUG_PRINT_DUMPSTK,   // print(call frame)
    OC_DEBUG_PRINT_DUMPMEM,   // print(A-B hex dump mem)
    OC_DEBUG_ASSERT,          // if (A == 0) sys.abort(AssertError)
    OC_DEBUG_BRK,             //
    OC_DEBUG_TICK,            // A = time

    OC_STACK_ALLOC,           // sb += imm32
    OC_STACK_FREE,            // sb -= imm32
    OC_STACK_LOAD_8U,         // A = stack[sb + imm32]
    OC_STACK_LOAD_8S,         // A = stack[sb + imm32]
    OC_STACK_LOAD_16U,        // A = stack[sb + imm32]
    OC_STACK_LOAD_16S,        // A = stack[sb + imm32]
    OC_STACK_LOAD_32U,        // A = stack[sb + imm32]
    OC_STACK_LOAD_32S,        // A = stack[sb + imm32]
    OC_STACK_LOAD_64,         // A = stack[sb + imm32]
    OC_STACK_STORE_8,         // stack[imm32] = A
    OC_STACK_STORE_16,        // stack[imm32] = A
    OC_STACK_STORE_32,        // stack[imm32] = A
    OC_STACK_STORE_64,        // stack[imm32] = A

    OC_MEMORY_ALLOC,          // A = malloc(B)
    OC_MEMORY_FREE,           // free(A)
    OC_MEMORY_LOAD_8U,        // A = memory[B]
    OC_MEMORY_LOAD_8S,        // A = memory[B]
    OC_MEMORY_LOAD_16U,       // A = memory[B]
    OC_MEMORY_LOAD_16S,       // A = memory[B]
    OC_MEMORY_LOAD_32U,       // A = memory[B]
    OC_MEMORY_LOAD_32S,       // A = memory[B]
    OC_MEMORY_LOAD_64,        // A = memory[B]
    OC_MEMORY_STORE_8,        // memory[A] = B
    OC_MEMORY_STORE_16,       // memory[A] = B
    OC_MEMORY_STORE_32,       // memory[A] = B
    OC_MEMORY_STORE_64,       // memory[A] = B
    OC_MEMORY_CPY,            // memcpy(A, B, C)
    OC_MEMORY_FILL,           // memfill<width = imm32>(A, B, C)
    OC_MEMORY_CMP,            // A = memcmp(B, C, D)

    OC_REG_MOVE_8,            // A = B
    OC_REG_MOVE_16,           // A = B
    OC_REG_MOVE_32,           // A = B
    OC_REG_MOVE_64,           // A = B
    OC_REG_SWAP,              // swap(A, B)
    OC_REG_LOAD_8U,           // A = imm8
    OC_REG_LOAD_8S,           // A = imm8
    OC_REG_LOAD_16U,          // A = imm16
    OC_REG_LOAD_16S,          // A = imm16
    OC_REG_LOAD_32U,          // A = imm32
    OC_REG_LOAD_32S,          // A = imm32
    OC_REG_LOAD_64,           // A = imm64

    OC_ALU_I64TOF64,          // A = B
    OC_ALU_U64TOF64,          // A = B
    OC_ALU_I64TOF32,          // A = B
    OC_ALU_U64TOF32,          // A = B
    OC_ALU_F64TOI64,          // A = B
    OC_ALU_F64TOU64,          // A = B
    OC_ALU_F32TOI64,          // A = B
    OC_ALU_F32TOU64,          // A = B
    OC_ALU_CVT_8S,            // A = B
    OC_ALU_CVT_16S,           // A = B
    OC_ALU_CVT_32S,           // A = B
    OC_ALU_INC,               // A = A + 1
    OC_ALU_DEC,               // A = A - 1
    OC_ALU_NEG,               // A = -A
    OC_ALU_NEG_F32,           // A = -A
    OC_ALU_NEG_F64,           // A = -A
    OC_ALU_ADD,               // A = B + C
    OC_ALU_ADD_F32,           // A = B + C
    OC_ALU_ADD_F64,           // A = B + C
    OC_ALU_SUB,               // A = B - C
    OC_ALU_SUB_F32,           // A = B - C
    OC_ALU_SUB_F64,           // A = B - C
    OC_ALU_MUL,               // A = B * C
    OC_ALU_MUL_F32,           // A = B * C
    OC_ALU_MUL_F64,           // A = B * C
    OC_ALU_DIV_U,             // A = B / C
    OC_ALU_DIV_S,             // A = B / C
    OC_ALU_DIV_F32,           // A = B / C
    OC_ALU_DIV_F64,           // A = B / C
    OC_ALU_MOD_U,             // A = B % C
    OC_ALU_MOD_S,             // A = B % C
    OC_ALU_AND,               // A = B & C
    OC_ALU_OR,                // A = B | C
    OC_ALU_XOR,               // A = B ^ C
    OC_ALU_NOT,               // A = ~B
    OC_ALU_SHL,               // A = B << C
    OC_ALU_SHR,               // A = B >>(L) RC
    OC_ALU_SAR,               // A = B >>(A) RC

    OC_CTRL_JMP,              // ip += offset
    OC_CTRL_JMPO,             // ip += A
    OC_CTRL_JZ,               // if (A == 0) ip += offset
    OC_CTRL_JNZ,              // if (A != 0) ip += offset
    OC_CTRL_JE,               // if (A == B) ip += offset
    OC_CTRL_JNE,              // if (A != B) ip += offset
    OC_CTRL_JL,               // if (A < B) ip += offset
    OC_CTRL_JLE,              // if (A <= B) ip += offset
    OC_CTRL_JB,               // if (A < B) ip += offset
    OC_CTRL_JBE,              // if (A <= B) ip += offset
    OC_CTRL_F32JE,            // if (A == B) ip += offset
    OC_CTRL_F32JNE,           // if (A != B) ip += offset
    OC_CTRL_F32JL,            // if (A < B) ip += offset
    OC_CTRL_F32JLE,           // if (A <= B) ip += offset
    OC_CTRL_F64JE,            // if (A == B) ip += offset
    OC_CTRL_F64JNE,           // if (A != B) ip += offset
    OC_CTRL_F64JL,            // if (A < B) ip += offset
    OC_CTRL_F64JLE,           // if (A <= B) ip += offset
    OC_CTRL_INC_JNE,          // A = A + 1 if (A != RB) ip += offset
    OC_CTRL_DEC_JNZ,          // A = A - 1 if (A != 0) ip += offset
    OC_CTRL_CALL,             // ip += offset base += shift
    OC_CTRL_RET,              // ip = ret_addr base = ret_base

    OC_UNKOWN = std::numeric_limits<std::uint8_t>::max(),
  };

  using Register = std::uint64_t;

  using Instruction = std::uint8_t;

  const std::flat_map<std::string_view, OpCode> opcodes {
    {"sys.exit",      OpCode::OC_SYS_EXIT},
    {"sys.abort",     OpCode::OC_SYS_ABORT},

    {"debug.printg",  OpCode::OC_DEBUG_PRINT_G},
    {"debug.printf",  OpCode::OC_DEBUG_PRINT_F},
    {"debug.prints",  OpCode::OC_DEBUG_PRINT_S},
    {"debug.regs",    OpCode::OC_DEBUG_PRINT_REGS},
    {"debug.dumpstk", OpCode::OC_DEBUG_PRINT_DUMPSTK},
    {"debug.dumpmem", OpCode::OC_DEBUG_PRINT_DUMPMEM},
    {"debug.assert",  OpCode::OC_DEBUG_ASSERT},
    {"debug.brk",     OpCode::OC_DEBUG_BRK},
    {"debug.tick",   OpCode::OC_DEBUG_TICK},

    {"stk.alloc",    OpCode::OC_STACK_ALLOC},
    {"stk.free",     OpCode::OC_STACK_FREE},
    {"stk.load8u",   OpCode::OC_STACK_LOAD_8U},
    {"stk.load8s",   OpCode::OC_STACK_LOAD_8S},
    {"stk.load16u",  OpCode::OC_STACK_LOAD_16U},
    {"stk.load16s",  OpCode::OC_STACK_LOAD_16S},
    {"stk.load32u",  OpCode::OC_STACK_LOAD_32U},
    {"stk.load32s",  OpCode::OC_STACK_LOAD_32S},
    {"stk.load64",   OpCode::OC_STACK_LOAD_64},
    {"stk.store8",   OpCode::OC_STACK_STORE_8},
    {"stk.store16",  OpCode::OC_STACK_STORE_16},
    {"stk.store32",  OpCode::OC_STACK_STORE_32},
    {"stk.store64",  OpCode::OC_STACK_STORE_64},
    
    {"mem.alloc",    OpCode::OC_MEMORY_ALLOC},
    {"mem.free",     OpCode::OC_MEMORY_FREE},
    {"mem.load8u",   OpCode::OC_MEMORY_LOAD_8U},
    {"mem.load8s",   OpCode::OC_MEMORY_LOAD_8S},
    {"mem.load16u",  OpCode::OC_MEMORY_LOAD_16U},
    {"mem.load16s",  OpCode::OC_MEMORY_LOAD_16S},
    {"mem.load32u",  OpCode::OC_MEMORY_LOAD_32U},
    {"mem.load32s",  OpCode::OC_MEMORY_LOAD_32S},
    {"mem.load64",   OpCode::OC_MEMORY_LOAD_64},
    {"mem.store8",   OpCode::OC_MEMORY_STORE_8},
    {"mem.store16",  OpCode::OC_MEMORY_STORE_16},
    {"mem.store32",  OpCode::OC_MEMORY_STORE_32},
    {"mem.store64",  OpCode::OC_MEMORY_STORE_64},
    {"mem.cpy",      OpCode::OC_MEMORY_CPY},
    {"mem.fill",     OpCode::OC_MEMORY_FILL},
    {"mem.cmp",      OpCode::OC_MEMORY_CMP},

    {"reg.move8",    OpCode::OC_REG_MOVE_8},
    {"reg.move16",   OpCode::OC_REG_MOVE_16},
    {"reg.move32",   OpCode::OC_REG_MOVE_32},
    {"reg.move64",   OpCode::OC_REG_MOVE_64},
    {"reg.swap",     OpCode::OC_REG_SWAP},
    {"reg.load8u",   OpCode::OC_REG_LOAD_8U},
    {"reg.load8s",   OpCode::OC_REG_LOAD_8S},
    {"reg.load16u",  OpCode::OC_REG_LOAD_16U},
    {"reg.load16s",  OpCode::OC_REG_LOAD_16S},
    {"reg.load32u",  OpCode::OC_REG_LOAD_32U},
    {"reg.load32s",  OpCode::OC_REG_LOAD_32S},
    {"reg.load64",   OpCode::OC_REG_LOAD_64},

    {"alu.i64tof64", OpCode::OC_ALU_I64TOF64},
    {"alu.u64tof64", OpCode::OC_ALU_U64TOF64},
    {"alu.f64toi32", OpCode::OC_ALU_I64TOF32},
    {"alu.f64tou32", OpCode::OC_ALU_U64TOF32},
    {"alu.i64tof64", OpCode::OC_ALU_F64TOI64},
    {"alu.u64tof64", OpCode::OC_ALU_F64TOU64},
    {"alu.f32toi64", OpCode::OC_ALU_F32TOI64},
    {"alu.f32tou64", OpCode::OC_ALU_F32TOU64},
    {"alu.inc",      OpCode::OC_ALU_INC},
    {"alu.dec",      OpCode::OC_ALU_DEC},
    {"alu.neg",      OpCode::OC_ALU_NEG},
    {"alu.fneg",     OpCode::OC_ALU_NEG_F32},
    {"alu.fneg",     OpCode::OC_ALU_NEG_F64},
    {"alu.add",      OpCode::OC_ALU_ADD},
    {"alu.f32add",   OpCode::OC_ALU_ADD_F32},
    {"alu.f64add",   OpCode::OC_ALU_ADD_F64},
    {"alu.sub",      OpCode::OC_ALU_SUB},
    {"alu.f32sub",   OpCode::OC_ALU_SUB_F32},
    {"alu.f64sub",   OpCode::OC_ALU_SUB_F64},
    {"alu.mul",      OpCode::OC_ALU_MUL},
    {"alu.f32mul",   OpCode::OC_ALU_MUL_F32},
    {"alu.f64mul",   OpCode::OC_ALU_MUL_F64},
    {"alu.divu",     OpCode::OC_ALU_DIV_U},
    {"alu.divs",     OpCode::OC_ALU_DIV_S},
    {"alu.f32div",   OpCode::OC_ALU_DIV_F32},
    {"alu.f64div",   OpCode::OC_ALU_DIV_F64},
    {"alu.modu",     OpCode::OC_ALU_MOD_U},
    {"alu.mods",     OpCode::OC_ALU_MOD_S},
    {"alu.and",      OpCode::OC_ALU_AND},
    {"alu.or",       OpCode::OC_ALU_OR},
    {"alu.xor",      OpCode::OC_ALU_XOR},
    {"alu.not",      OpCode::OC_ALU_NOT},
    {"alu.shl",      OpCode::OC_ALU_SHL},
    {"alu.shr",      OpCode::OC_ALU_SHR},
    {"alu.sar",      OpCode::OC_ALU_SAR},

    {"ctrl.jmp",     OpCode::OC_CTRL_JMP},
    {"ctrl.jmpo",    OpCode::OC_CTRL_JMPO},
    {"ctrl.jz",      OpCode::OC_CTRL_JZ},
    {"ctrl.jnz",     OpCode::OC_CTRL_JNZ},
    {"ctrl.je",      OpCode::OC_CTRL_JE},
    {"ctrl.jne",     OpCode::OC_CTRL_JNE},
    {"ctrl.jl",      OpCode::OC_CTRL_JL},
    {"ctrl.jle",     OpCode::OC_CTRL_JLE},
    {"ctrl.jb",      OpCode::OC_CTRL_JB},
    {"ctrl.jbe",     OpCode::OC_CTRL_JBE},
    {"ctrl.f32je",   OpCode::OC_CTRL_F32JE},
    {"ctrl.f32jne",  OpCode::OC_CTRL_F32JNE},
    {"ctrl.f32jl",   OpCode::OC_CTRL_F32JL},
    {"ctrl.f32jle", OpCode::OC_CTRL_F32JLE},
    {"ctrl.f64je",  OpCode::OC_CTRL_F64JE},
    {"ctrl.f64jne", OpCode::OC_CTRL_F64JNE},
    {"ctrl.f64jl",  OpCode::OC_CTRL_F64JL},
    {"ctrl.f64jle", OpCode::OC_CTRL_F64JLE},
    {"ctrl.ijne",   OpCode::OC_CTRL_INC_JNE},
    {"ctrl.djnz",   OpCode::OC_CTRL_DEC_JNZ},
    {"ctrl.call",   OpCode::OC_CTRL_CALL},
    {"ctrl.ret",    OpCode::OC_CTRL_RET}
  };

  OpCode find_opcode(std::string_view text) {
    if (
      auto it = opcodes.find(text); 
      it != opcodes.end()
    ) [[likely]] return it->second;
    return OpCode::OC_UNKOWN;
  }
}