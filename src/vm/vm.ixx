module;
#include <cstddef>
export module tona.vm;

import std;

import tona.config;
import tona.byte;
import tona.opcode;

export namespace Tona {

  class VM {
    public:
      VM(std::unique_ptr<std::vector<std::uint8_t>> const_mem)
      : gp_regs(std::make_unique<GPRegister[]>(TVM_MAX_REG_SIZE)),
        fp_regs(std::make_unique<FPRegister[]>(TVM_MAX_REG_SIZE)),
        call_stack(std::make_unique<CallFrame[]>(TVM_MAX_CALL_FRAME_SIZE)),
        mem(std::move(const_mem)),
        boundary(const_mem->size())
      {}
      ~VM() {}

      void run(const Instruction* is) {
        const auto* i = is;
        std::size_t base = 0;

        constexpr void* labels[] = {
          &&v_print_g,&&v_print_f, 
          
          &&v_move, &&v_fmove,
          &&v_load8, &&v_load16,
          &&v_load32, &&v_iload8,
          &&v_iload16, &&v_iload32,
          &&v_load,
          &&v_fload32, &&v_fload,
          &&v_inc, &&v_dec,
          &&v_neg, &&v_add, 
          &&v_sub,&&v_mul, 
          &&v_div, &&v_divs,
          &&v_mod, &&v_mods,
          &&v_fneg, &&v_fadd,
          &&v_fsub, &&v_fmul,
          &&v_fdiv, &&v_fmax, 
          &&v_fmin, &&v_fsqrt,

          &&v_itof, &&v_utof,
          &&v_ftoi, &&v_ftou,

          &&v_jmp, &&v_jmpo,
          &&v_je, &&v_jne,
          &&v_jg, &&v_jge,
          &&v_jl, &&v_jle,
          &&v_ja, &&v_jae,
          &&v_jb, &&v_jbe,
          &&v_fje, &&v_fjne,
          &&v_fjg, &&v_fjge,
          &&v_fjl, &&v_fjle,

          &&v_and, &&v_or,
          &&v_xor, &&v_not,

          &&v_shl, &&v_shr,
          &&v_sar,

          &&v_sext8, &&v_sext16,
          &&v_sext32,

          &&v_call, &&v_ret,

          &&v_ldm8, &&v_ldm16,
          &&v_ldm32, &&v_ildm8,
          &&v_ildm16, &&v_ildm32,
          &&v_ldm,
          &&v_fldm32, &&v_fldm,
          &&v_stm8, &&v_stm16,
          &&v_stm32, &&v_stm,
          &&v_fstm32, &&v_fstm
          //&&v_grow, 
        };

        goto *labels[*i];

        v_move:    goto *labels[move_op<GPRegister>(base, ++i)];
        v_fmove:   goto *labels[move_op<FPRegister>(base, ++i)];
        v_load8:   goto *labels[load_op<GPRegister, std::uint8_t>(base, ++i)];
        v_load16:  goto *labels[load_op<GPRegister, std::uint16_t>(base, ++i)];
        v_load32:  goto *labels[load_op<GPRegister, std::uint32_t>(base, ++i)];
        v_iload8:  goto *labels[load_op<GPRegister, std::int8_t>(base, ++i)];
        v_iload16: goto *labels[load_op<GPRegister, std::int16_t>(base, ++i)];
        v_iload32: goto *labels[load_op<GPRegister, std::int32_t>(base, ++i)];
        v_load:    goto *labels[load_op<GPRegister, std::uint64_t>(base, ++i)];
        v_fload32: goto *labels[load_op<FPRegister, float>(base, ++i)];
        v_fload:   goto *labels[load_op<FPRegister, double>(base, ++i)];
        v_add:     goto *labels[bin_op<GPRegister, std::plus<>>(base, ++i)];
        v_sub:     goto *labels[bin_op<GPRegister, std::minus<>>(base, ++i)];
        v_mul:     goto *labels[bin_op<GPRegister, std::multiplies<>>(base, ++i)];
        v_div:     goto *labels[bin_op<GPRegister, std::divides<>>(base, ++i)];
        v_divs:    goto *labels[bin_op<GPRegister, std::divides<>, std::int64_t>(base, ++i)];
        v_mod:     goto *labels[bin_op<GPRegister, std::modulus<>>(base, ++i)];
        v_mods:    goto *labels[bin_op<GPRegister, std::modulus<>, std::int64_t>(base, ++i)];
        v_fadd:    goto *labels[bin_op<FPRegister, std::plus<>>(base, ++i)];
        v_fsub:    goto *labels[bin_op<FPRegister, std::minus<>>(base, ++i)];
        v_fmul:    goto *labels[bin_op<FPRegister, std::multiplies<>>(base, ++i)];
        v_fdiv:    goto *labels[bin_op<FPRegister, std::divides<>>(base, ++i)];
        v_neg:     goto *labels[un_op<GPRegister, std::negate<>>(base, ++i)];
        v_fneg:    goto *labels[un_op<FPRegister, std::negate<>>(base, ++i)];
        v_fmax:    goto *labels[bin_op<FPRegister, maximum>(base, ++i)];
        v_fmin:    goto *labels[bin_op<FPRegister, minimum>(base, ++i)];
        v_fsqrt:   goto *labels[un_op<FPRegister, square_root>(base, ++i)];

        v_and:     goto *labels[bin_op<GPRegister, std::bit_and<>>(base, ++i)];
        v_or:      goto *labels[bin_op<GPRegister, std::bit_or<>>(base, ++i)];
        v_xor:     goto *labels[bin_op<GPRegister, std::bit_xor<>>(base, ++i)];
        v_not:     goto *labels[un_op<GPRegister, std::bit_not<>>(base, ++i)];

        v_shl:     goto *labels[bin_op<GPRegister, bit_shift_left>(base, ++i)];
        v_shr:     goto *labels[bin_op<GPRegister, bit_shift_right>(base, ++i)];
        v_sar:     goto *labels[bin_op<GPRegister, bit_shift_right, std::int64_t>(base, ++i)];

        v_sext8:   goto *labels[un_op<GPRegister, sign_extend_8>(base, ++i)];
        v_sext16:  goto *labels[un_op<GPRegister, sign_extend_16>(base, ++i)];
        v_sext32:  goto *labels[un_op<GPRegister, sign_extend_32>(base, ++i)];

        v_inc:     goto *labels[un_op<GPRegister, inc>(base, ++i)];
        v_dec:     goto *labels[un_op<GPRegister, dec>(base, ++i)];

        v_itof: goto *labels[tc_op<FPRegister, GPRegister, std::int64_t>(base, ++i)];
        v_utof: goto *labels[tc_op<FPRegister, GPRegister>(base, ++i)];
        v_ftoi: goto *labels[tc_op<GPRegister, FPRegister, std::int64_t>(base, ++i)];
        v_ftou: goto *labels[tc_op<GPRegister, FPRegister>(base, ++i)];

        v_jmp: {
          std::int32_t offset; 
          store<std::int32_t>(offset, ++i);
          goto *labels[*(i += offset)];
        }
        v_jmpo: {
          const auto& A = reg<GPRegister>(base, *++i);
          goto *labels[*(i += A)];
        }
        v_je:      goto *labels[branch<GPRegister, std::equal_to<>>(base, ++i)];
        v_jne:     goto *labels[branch<GPRegister, std::not_equal_to<>>(base, ++i)];
        v_jl:      goto *labels[branch<GPRegister, std::less<>>(base, ++i)];
        v_jle:     goto *labels[branch<GPRegister, std::less_equal<>>(base, ++i)];
        v_jg:      goto *labels[branch<GPRegister, std::greater<>>(base, ++i)];
        v_jge:     goto *labels[branch<GPRegister, std::greater_equal<>>(base, ++i)];
        v_ja:      goto *labels[branch<GPRegister, std::greater_equal<>, std::int64_t>(base, ++i)];
        v_jae:     goto *labels[branch<GPRegister, std::greater_equal<>, std::int64_t>(base, ++i)];
        v_jb:      goto *labels[branch<GPRegister, std::greater_equal<>, std::int64_t>(base, ++i)];
        v_jbe:     goto *labels[branch<GPRegister, std::greater_equal<>, std::int64_t>(base, ++i)];
        v_fje:     goto *labels[branch<FPRegister, std::equal_to<>>(base, ++i)];
        v_fjne:    goto *labels[branch<FPRegister, std::not_equal_to<>>(base, ++i)];
        v_fjl:     goto *labels[branch<FPRegister, std::less<>>(base, ++i)];
        v_fjle:    goto *labels[branch<FPRegister, std::less_equal<>>(base, ++i)];
        v_fjg:     goto *labels[branch<FPRegister, std::greater<>>(base, ++i)];
        v_fjge:    goto *labels[branch<FPRegister, std::greater_equal<>>(base, ++i)];

        v_ldm8:    goto *labels[load_mem<GPRegister, std::uint8_t>(base, ++i)];
        v_ldm16:   goto *labels[load_mem<GPRegister, std::uint16_t>(base, ++i)];
        v_ldm32:   goto *labels[load_mem<GPRegister, std::uint32_t>(base, ++i)];
        v_ildm8:   goto *labels[load_mem<GPRegister, std::int8_t>(base, ++i)];
        v_ildm16:  goto *labels[load_mem<GPRegister, std::int16_t>(base, ++i)];
        v_ildm32:  goto *labels[load_mem<GPRegister, std::int32_t>(base, ++i)];
        v_ldm:     goto *labels[load_mem<GPRegister, std::uint64_t>(base, ++i)];
        v_fldm32:  goto *labels[load_mem<FPRegister, float>(base, ++i)];
        v_fldm:    goto *labels[load_mem<FPRegister, double>(base, ++i)];
        v_stm8:    goto *labels[store_mem<GPRegister, std::uint8_t>(base, ++i)];
        v_stm16:   goto *labels[store_mem<GPRegister, std::uint16_t>(base, ++i)];
        v_stm32:   goto *labels[store_mem<GPRegister, std::uint32_t>(base, ++i)];
        v_stm:     goto *labels[store_mem<GPRegister, std::uint64_t>(base, ++i)];
        v_fstm32:  goto *labels[store_mem<FPRegister, float>(base, ++i)];
        v_fstm:    goto *labels[store_mem<FPRegister, double>(base, ++i)];

        v_grow: {
          auto& RA = reg<GPRegister>(base, *++i);
          const auto& RB = reg<GPRegister>(base, *++i);

          std::size_t old_size = mem->size();
          mem->resize(old_size + RB);
          RA = old_size;
          goto *labels[*++i];
        }

        v_call: {
          std::int32_t offset; 
          store<std::int32_t>(offset, ++i);
          std::uint8_t shift = *i++;
          call_stack[frame_idx++] = CallFrame{
            .ret_addr = i,
            .base = base
          };
          base += shift;
          goto *labels[*(i += offset)];
        }

        v_ret: {
          if (frame_idx == 0)
            return;
          const auto& frame = call_stack[--frame_idx];
          base = frame.base;
          i = frame.ret_addr;
          goto *labels[*i];
        }

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
        const auto& B = reg<Reg>(base, *++i);
        A = B;
        return *++i;
      }

      template <typename Reg, typename Op>
      [[nodiscard]] [[gnu::always_inline]] inline std::uint8_t un_op(const std::size_t base, const Instruction*& i) noexcept {
        auto& A = reg<Reg>(base, *i);
        const auto& B = reg<Reg>(base, *++i);
        A = Op{}(B);
        return *++i;
      }

      template <typename Reg, typename Op, typename T = Reg>
      [[nodiscard]] [[gnu::always_inline]] inline std::uint8_t bin_op(const std::size_t base, const Instruction*& i) noexcept {
        auto& A = reg<Reg>(base, *i);
        const auto& B = reg<Reg>(base, *++i);
        const auto& C = reg<Reg>(base, *++i);
        A = Op{}(static_cast<T>(B), static_cast<T>(C));
        return *++i;
      }

      template <typename Reg, typename Op, typename T = Reg>
      [[nodiscard]] [[gnu::always_inline]] inline std::uint8_t branch(const std::size_t base, const Instruction*& i) noexcept {
        const auto& A = reg<Reg>(base, *i);
        const auto& B = reg<Reg>(base, *++i);
        std::int32_t offset;
        store<std::int32_t>(offset, ++i);
        if (Op{}(static_cast<T>(A), static_cast<T>(B)))
          i += offset;
        return *i;
      }

      template <typename Reg, typename T>
      [[nodiscard]] [[gnu::always_inline]] inline std::uint8_t load_mem(const std::size_t base, const Instruction*& i) noexcept {
        auto& A = reg<Reg>(base, *i);
        const auto& B = reg<GPRegister>(base, *++i);
        A = read_mem<T>(B);
        return *++i;
      }

      template <typename Reg, typename T>
      [[nodiscard]] [[gnu::always_inline]] inline std::uint8_t store_mem(const std::size_t base, const Instruction*& i) noexcept {
        const auto& A = reg<GPRegister>(base, *i);
        const auto& B = reg<Reg>(base, *++i);
        write_mem(A, B);
        return *++i;
      }

      template <typename DReg, typename SReg, typename T = DReg>
      [[nodiscard]] [[gnu::always_inline]] inline std::uint8_t tc_op(const std::size_t base, const Instruction*& i) noexcept {
        auto& A = reg<DReg>(base, *i);
        const auto& B = reg<SReg>(base, *++i);
        A = static_cast<DReg>(static_cast<T>(B));
        return *++i;
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
        T val;
        std::memcpy(&val, src, sizeof(T));
        dst = val;
        src += sizeof(T);
      }

      template <typename T>
      [[gnu::always_inline]] inline T read_mem(std::size_t addr) const noexcept {
        if (addr + sizeof(T) > mem->size()) [[unlikely]] {
          std::println("error: Out of bounds read 0x{:X}", addr);
          std::exit(1);
          // test
        }

        T val;
        std::memcpy(&val, mem->data() + addr, sizeof(T));
        return val; 
      }

      template <typename T>
      [[gnu::always_inline]] inline void write_mem(std::size_t addr, const T& val) noexcept {
        if (addr < boundary) [[unlikely]] {
          std::println("error: Write to Read-Only memory 0x{:X}", addr);
          std::exit(1);
          // test
        }

        if (addr + sizeof(T) > mem->size()) [[unlikely]] {
          std::println("error: Out of bounds write 0x{:X}", addr);
          std::exit(1);
          // test
        }

        std::memcpy(mem->data() + addr, &val, sizeof(T));
      }

    private:
      std::size_t frame_idx = 0;
      std::size_t boundary;
      std::unique_ptr<std::vector<std::uint8_t>> mem;
      std::unique_ptr<GPRegister[]> gp_regs;
      std::unique_ptr<FPRegister[]> fp_regs;
      std::unique_ptr<CallFrame[]> call_stack;
  };

}