export module tona.vm;

import std;

import tona.config;
import tona.byte;
import tona.functional;
import tona.opcode;
import tona.error;

export namespace Tona {

  class VM {
    public:
      VM(std::vector<std::uint8_t>& const_mem) 
        : mem(const_mem) {}
      VM(const VM&) = delete;
      VM& operator=(const VM&) = delete;
      VM (VM&&) = default;
      VM& operator=(VM&&) = delete;
      ~VM() = default;

    VMErrorType run(const Instruction* ip) {
      constexpr void* labels[] = {
        #include "vm_label.inc"
      };

      goto *labels[*ip];

      debug_printg:   goto *labels[dprint(++ip)];
      debug_prints:   goto *labels[sprint(++ip)];
      debug_printgs:  goto *labels[dprints(++ip)];
      debug_dumpstk:  goto *labels[dumpstk(++ip)];
      debug_dumpmem:  goto *labels[dumpmem(++ip)];

      sys_abort:
        code = static_cast<VMErrorType>(reg(*++ip));

      sys_exit:
        return code;
    }

    private:
      [[nodiscard]] std::uint8_t dprint(const Instruction*& ip) {
        const auto& A = reg(*ip);
        std::println("regs[{}]: 0x{:X}", *ip, A);
        return *++ip;
      }

      [[nodiscard]] std::uint8_t sprint(const Instruction*& ip) {
        const auto& A = reg(*ip);
        std::println("regs[{}]: \"{}\"", *ip, reinterpret_cast<char*>(&mem[A]));
        return *++ip;
      }

      [[nodiscard]] std::uint8_t dprints(const Instruction*& ip) {
        std::println("start: {}", rb);
        for (std::size_t i = 0; i < 256; i++) {
          const auto& A = reg(i);
          std::println("regs[{}]: 0x{:X}", *ip, A);
        }
        std::println("end: {}", rb + 256);
        return *ip;
      }

      [[nodiscard]] std::uint8_t dumpstk(const Instruction*& ip) {
        return *ip;
      }

      [[nodiscard]] std::uint8_t dumpmem(const Instruction*& ip) {
        return *ip;
      }

    private:
      [[nodiscard]] [[gnu::always_inline]] inline Register& reg(const Instruction i) noexcept {
        return regs.get()[rb + i];
      }

    private:
      VMErrorType code = VMErrorType::VMET_NONE;
      std::size_t sp = 0;
      std::size_t rb = 0;
      std::unique_ptr<Register> regs;
      std::unique_ptr<std::uint8_t> stack;
      std::vector<std::uint8_t>& mem;
  };

}