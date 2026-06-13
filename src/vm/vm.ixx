export module tona.vm;

import std;

import tona.config;
import tona.byte;
import tona.opcode;

export namespace Tona {

  class VM {
    public:
      VM()
      : gp_regs(std::make_unique<GPRegister[]>(TVM_MAX_REG_SIZE)),
        fp_regs(std::make_unique<FPRegister[]>(TVM_MAX_REG_SIZE))
      {}
      ~VM() {}

      void run(const Instruction* is) {
        const auto* i = is;
        std::size_t base = 0;

        constexpr void* labels[] = {
          &&v_end, &&v_print_g,
          &&v_print_f, &&v_move, 
          &&v_fmove,
          &&v_load8, &&v_load16,
          &&v_load32,&&v_load, 
          &&v_fload32, &&v_fload,
          &&v_add, &&v_sub,
          &&v_mul, &&v_div,
          &&v_mod, &&v_fadd,
          &&v_fsub, &&v_fmul,
          &&v_fdiv, &&v_jmp,
          &&v_jeq, &&v_jne,
          &&v_jlt, &&v_jle,
          &&v_jgt, &&v_jge
        };

        goto *labels[*i];

        v_move:    goto *labels[move_op<GPRegister>(base, ++i)];
        v_fmove:   goto *labels[move_op<FPRegister>(base, ++i)];
        v_load8:   goto *labels[load_op<GPRegister, std::uint8_t>(base, ++i)];
        v_load16:  goto *labels[load_op<GPRegister, std::uint16_t>(base, ++i)];
        v_load32:  goto *labels[load_op<GPRegister, std::uint32_t>(base, ++i)];
        v_load:    goto *labels[load_op<GPRegister, std::uint64_t>(base, ++i)];
        v_fload32: goto *labels[load_op<FPRegister, float>(base, ++i)];
        v_fload:   goto *labels[load_op<FPRegister, double>(base, ++i)];
        v_add:     goto *labels[bin_op<GPRegister, std::plus<>>(base, ++i)];
        v_sub:     goto *labels[bin_op<GPRegister, std::minus<>>(base, ++i)];
        v_mul:     goto *labels[bin_op<GPRegister, std::multiplies<>>(base, ++i)];
        v_div:     goto *labels[bin_op<GPRegister, std::divides<>>(base, ++i)];
        v_mod:     goto *labels[bin_op<GPRegister, std::modulus<>>(base, ++i)];
        v_fadd:    goto *labels[bin_op<FPRegister, std::plus<>>(base, ++i)];
        v_fsub:    goto *labels[bin_op<FPRegister, std::minus<>>(base, ++i)];
        v_fmul:    goto *labels[bin_op<FPRegister, std::multiplies<>>(base, ++i)];
        v_fdiv:    goto *labels[bin_op<FPRegister, std::divides<>>(base, ++i)];

        v_jmp: {
          std::int32_t offset; 
          store<std::int32_t>(offset, ++i);
          goto *labels[*(i += offset)];
        }
        v_jeq:     goto *labels[branch<GPRegister, std::equal_to<>>(base, ++i)];
        v_jne:     goto *labels[branch<GPRegister, std::not_equal_to<>>(base, ++i)];
        v_jlt:     goto *labels[branch<GPRegister, std::less<>>(base, ++i)];
        v_jle:     goto *labels[branch<GPRegister, std::less_equal<>>(base, ++i)];
        v_jgt:     goto *labels[branch<GPRegister, std::greater<>>(base, ++i)];
        v_jge:     goto *labels[branch<GPRegister, std::greater_equal<>>(base, ++i)];

        v_print_g: {
          auto& A = reg<GPRegister>(base, *++i);
          std::println("gpreg{}: {}", base + cast_u8(*i),  A);
          goto *labels[*++i];
        }

        v_print_f: {
          auto& A = reg<FPRegister>(base, *++i);
          std::println("fpreg{}: {}", base + cast_u8(*i),  A);
          goto *labels[*++i];
        }

        v_end:
          std::println("end");
          return;
      }

    private:
      template <typename Reg, typename T>
      [[nodiscard]] [[gnu::always_inline]] inline std::uint8_t load_op(const std::size_t base, const Instruction*& i) noexcept {
        auto& A = reg<Reg>(base, *i);
        store<T>(A, ++i);
        return *i;
      }

      template <typename Reg>
      [[nodiscard]] [[gnu::always_inline]] inline std::uint8_t move_op(const std::size_t base, const Instruction*& i) noexcept {
        auto& A = reg<Reg>(base, *i);
        auto& B = reg<Reg>(base, *++i);
        A = B;
        return *++i;
      }

      template <typename Reg, typename Op>
      [[nodiscard]] [[gnu::always_inline]] inline std::uint8_t bin_op(const std::size_t base, const Instruction*& i) noexcept {
        auto& A = reg<Reg>(base, *i);
        auto& B = reg<Reg>(base, *++i);
        auto& C = reg<Reg>(base, *++i);
        A = Op{}(B, C);
        return *++i;
      }

      template <typename Reg, typename Op>
      [[nodiscard]] [[gnu::always_inline]] inline std::uint8_t branch(const std::size_t base, const Instruction*& i) noexcept {
        auto& A = reg<Reg>(base, *i);
        auto& B = reg<Reg>(base, *++i);
        std::int32_t offset;
        store<std::int32_t>(offset, ++i);
        if (Op{}(A, B))
          i += offset;
        return *i;
      } 

    private:
      template <typename T>
      [[nodiscard]] [[gnu::always_inline]] inline T& reg(const std::size_t base, const Instruction i) noexcept {
        if constexpr (std::is_same_v<T, FPRegister>)
          return fp_regs[base + i];
        else if constexpr (std::is_same_v<T, GPRegister>)
          return gp_regs[base + i];
        else static_assert(false, "unkown register");
      }

      template <typename T>
        requires (std::is_arithmetic_v<T>)
      [[gnu::always_inline]] inline void store(auto& dst, const Instruction*& src) noexcept {
        std::memcpy(&dst, src, sizeof(T));
        src += sizeof(T);
      }

    private:
      std::unique_ptr<GPRegister[]> gp_regs;
      std::unique_ptr<FPRegister[]> fp_regs;
  };

}