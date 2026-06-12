export module tona.vm;

import std;

import tona.config;
import tona.byte;
import tona.opcode;
import tona.chunk;

export namespace Tona {

  class VM {
    public:
      VM() {
        gp_regs = new GPRegister[TVM_MAX_REG_SIZE];
        fp_regs = new FPRegister[TVM_MAX_REG_SIZE];
      }
      ~VM() {
        delete[] gp_regs;
        delete[] fp_regs;
      }

      void run(const Instruction* is) {
        const auto* i = is;
        std::size_t base = 0;

        constexpr void* labels[] = {
          &&v_move, &&v_fmove,
          &&v_load, &&v_fload, 
          &&v_add, &&v_sub,
          &&v_mul, &&v_div,
          &&v_mod, &&v_fadd,
          &&v_fsub, &&v_fmul,
          &&v_fdiv
        };

        goto *labels[cast_u8(*i)];

        v_move: {
          auto& A = reg<GPRegister>(base, *++i);
          auto& B = reg<GPRegister>(base, *++i);
          A = B;
          goto *labels[cast_u8(*++i)];
        }

        v_fmove: {
          auto& A = reg<FPRegister>(base, *++i);
          auto& B = reg<FPRegister>(base, *++i);
          A = B;
          goto *labels[cast_u8(*++i)];
        }

        v_load: {
          auto& A = reg<GPRegister>(base, *++i);
          store<std::uint64_t>(A, ++i);
          goto *labels[cast_u8(*++i)];
        }

        v_fload: {
          auto& A = reg<FPRegister>(base, *++i);
          store<double>(A, ++i);
          goto *labels[cast_u8(*++i)];
        }

        v_add: {
          auto& A = reg<GPRegister>(base, *++i);
          auto& B = reg<GPRegister>(base, *++i);
          auto& C = reg<GPRegister>(base, *++i);
          A = B + C;
          goto *labels[cast_u8(*++i)];
        }

        v_sub: {
          auto& A = reg<GPRegister>(base, *++i);
          auto& B = reg<GPRegister>(base, *++i);
          auto& C = reg<GPRegister>(base, *++i);
          A = B - C;
          goto *labels[cast_u8(*++i)];
        }

        v_mul: {
          auto& A = reg<GPRegister>(base, *++i);
          auto& B = reg<GPRegister>(base, *++i);
          auto& C = reg<GPRegister>(base, *++i);
          A = B * C;
          goto *labels[cast_u8(*++i)];
        }

        v_div: {
          auto& A = reg<GPRegister>(base, *++i);
          auto& B = reg<GPRegister>(base, *++i);
          auto& C = reg<GPRegister>(base, *++i);
          A = B / C;
          goto *labels[cast_u8(*++i)];
        }

        v_mod: {
          auto& A = reg<GPRegister>(base, *++i);
          auto& B = reg<GPRegister>(base, *++i);
          auto& C = reg<GPRegister>(base, *++i);
          A = B % C;
          goto *labels[cast_u8(*++i)];
        }

        v_fadd: {
          auto& A = reg<FPRegister>(base, *++i);
          auto& B = reg<FPRegister>(base, *++i);
          auto& C = reg<FPRegister>(base, *++i);
          A = B + C;
          goto *labels[cast_u8(*++i)];
        }

        v_fsub: {
          auto& A = reg<FPRegister>(base, *++i);
          auto& B = reg<FPRegister>(base, *++i);
          auto& C = reg<FPRegister>(base, *++i);
          A = B - C;
          goto *labels[cast_u8(*++i)];
        }

        v_fmul: {
          auto& A = reg<FPRegister>(base, *++i);
          auto& B = reg<FPRegister>(base, *++i);
          auto& C = reg<FPRegister>(base, *++i);
          A = B * C;
          goto *labels[cast_u8(*++i)];
        }

        v_fdiv: {
          auto& A = reg<FPRegister>(base, *++i);
          auto& B = reg<FPRegister>(base, *++i);
          auto& C = reg<FPRegister>(base, *++i);
          A = B / C;
          goto *labels[cast_u8(*++i)];
        }
      }

    private:
      template <typename T>
      [[nodiscard]] [[gnu::always_inline]] inline T& reg(const std::size_t base, const Instruction i) noexcept {
        if constexpr (std::is_same_v<T, FPRegister>)
          return fp_regs[base + cast_u8(i)];
        else if constexpr (std::is_same_v<T, GPRegister>)
          return gp_regs[base + cast_u8(i)];
        else static_assert(false, "unkown register");
      }

      template <typename T>
        requires (std::is_arithmetic_v<T>)
      [[gnu::always_inline]] inline void store(auto& dst, const Instruction*& src) noexcept {
        std::memcpy(&dst, src, sizeof(T));
        src += sizeof(T);
      }

    private:
      GPRegister* gp_regs;
      FPRegister* fp_regs;
  };

}