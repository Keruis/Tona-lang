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
      VM(std::vector<std::uint8_t>& memory) 
        : regs(std::make_unique<Register[]>(TVM_MAX_REG_SIZE)),
          stack(std::make_unique<std::uint8_t[]>(TVM_MAX_STACK_SIZE)),
          mem(memory) {}
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

      stk_alloc:       goto *labels[stk_alloc(++ip)];
      stk_free:        goto *labels[stk_free(++ip)];
      stk_ld_8u:       goto *labels[stk_load<std::uint8_t>(++ip)];
      stk_ld_8s:       goto *labels[stk_load<std::int8_t>(++ip)];
      stk_ld_16u:      goto *labels[stk_load<std::uint16_t>(++ip)];
      stk_ld_16s:      goto *labels[stk_load<std::int16_t>(++ip)];
      stk_ld_32u:      goto *labels[stk_load<std::uint32_t>(++ip)];
      stk_ld_32s:      goto *labels[stk_load<std::int32_t>(++ip)];
      stk_ld_64:       goto *labels[stk_load<std::uint64_t>(++ip)];
      stk_st_8:        goto *labels[stk_store<std::uint8_t>(++ip)];
      stk_st_16:       goto *labels[stk_store<std::uint16_t>(++ip)];
      stk_st_32:       goto *labels[stk_store<std::uint32_t>(++ip)];
      stk_st_64:       goto *labels[stk_store<std::uint64_t>(++ip)];

      mem_alloc:       goto *labels[mem_alloc(++ip)];
      mem_free:        goto *labels[mem_free(++ip)];
      mem_ld_8u:       goto *labels[mem_load<std::uint8_t>(++ip)];
      mem_ld_8s:       goto *labels[mem_load<std::int8_t>(++ip)];
      mem_ld_16u:      goto *labels[mem_load<std::uint16_t>(++ip)];
      mem_ld_16s:      goto *labels[mem_load<std::int16_t>(++ip)];
      mem_ld_32u:      goto *labels[mem_load<std::uint32_t>(++ip)];
      mem_ld_32s:      goto *labels[mem_load<std::int32_t>(++ip)];
      mem_ld_64:       goto *labels[mem_load<std::uint64_t>(++ip)];
      mem_st_8:        goto *labels[mem_store<std::uint8_t>(++ip)];
      mem_st_16:       goto *labels[mem_store<std::uint16_t>(++ip)];
      mem_st_32:       goto *labels[mem_store<std::uint32_t>(++ip)];
      mem_st_64:       goto *labels[mem_store<std::uint64_t>(++ip)];
      mem_cpy:         goto *labels[mem_cpy(++ip)];
      mem_set:         goto *labels[mem_set(++ip)];
      mem_cmp:         goto *labels[mem_cmp(++ip)];

      reg_move_8:      goto *labels[move<std::uint8_t>(++ip)];
      reg_move_16:     goto *labels[move<std::uint16_t>(++ip)];
      reg_move_32:     goto *labels[move<std::uint32_t>(++ip)];
      reg_move_64:     goto *labels[move<std::uint64_t>(++ip)];
      reg_swap:        goto *labels[swap(++ip)];
      reg_ld_8u:       goto *labels[reg_load<std::uint8_t>(++ip)];
      reg_ld_8s:       goto *labels[reg_load<std::int8_t>(++ip)];
      reg_ld_16u:      goto *labels[reg_load<std::uint16_t>(++ip)];
      reg_ld_16s:      goto *labels[reg_load<std::int16_t>(++ip)];
      reg_ld_32u:      goto *labels[reg_load<std::uint32_t>(++ip)];
      reg_ld_32s:      goto *labels[reg_load<std::int32_t>(++ip)];
      reg_ld_64:       goto *labels[reg_load<std::uint64_t>(++ip)];

      alu_cvt_64s_f64: goto *labels[cvt<std::int64_t, double>(++ip)];
      alu_cvt_64u_f64: goto *labels[cvt<std::uint64_t, double>(++ip)];
      alu_cvt_64s_f32: goto *labels[cvt<std::int64_t, float>(++ip)];
      alu_cvt_64u_f32: goto *labels[cvt<std::uint64_t, float>(++ip)];
      alu_cvt_f64_64s: goto *labels[cvt<double, std::int64_t>(++ip)];
      alu_cvt_f64_64u: goto *labels[cvt<double, std::uint64_t>(++ip)];
      alu_cvt_f32_64s: goto *labels[cvt<float, std::int64_t>(++ip)];
      alu_cvt_f32_64u: goto *labels[cvt<float, std::uint64_t>(++ip)];
      alu_cvt_8s_64s:  goto *labels[cvt<std::int8_t, std::int64_t>(++ip)];
      alu_cvt_16s_64s: goto *labels[cvt<std::int16_t, std::int64_t>(++ip)];
      alu_cvt_32s_64s: goto *labels[cvt<std::int32_t, std::int64_t>(++ip)];
      alu_inc:         goto *labels[un_op<inc, std::uint64_t>(++ip)];
      alu_dec:         goto *labels[un_op<dec, std::uint64_t>(++ip)];
      alu_neg:         goto *labels[un_op<std::negate<>, std::uint64_t>(++ip)];
      alu_neg_f32:     goto *labels[un_op<std::negate<>, float>(++ip)];
      alu_neg_f64:     goto *labels[un_op<std::negate<>, double>(++ip)];
      alu_add:         goto *labels[bin_op<std::plus<>, std::uint64_t>(++ip)];
      alu_add_f32:     goto *labels[bin_op<std::plus<>, float>(++ip)];
      alu_add_f64:     goto *labels[bin_op<std::plus<>, double>(++ip)];
      alu_sub:         goto *labels[bin_op<std::minus<>, std::uint64_t>(++ip)];
      alu_sub_f32:     goto *labels[bin_op<std::minus<>, float>(++ip)];
      alu_sub_f64:     goto *labels[bin_op<std::minus<>, double>(++ip)];
      alu_mul:         goto *labels[bin_op<std::multiplies<>, std::uint64_t>(++ip)];
      alu_mul_f32:     goto *labels[bin_op<std::multiplies<>, float>(++ip)];
      alu_mul_f64:     goto *labels[bin_op<std::multiplies<>, double>(++ip)];
      alu_div_u:       goto *labels[bin_op<std::divides<>, std::uint64_t>(++ip)];
      alu_div_s:       goto *labels[bin_op<std::divides<>, std::int64_t>(++ip)];
      alu_div_f32:     goto *labels[bin_op<std::divides<>, float>(++ip)];
      alu_div_f64:     goto *labels[bin_op<std::divides<>, double>(++ip)];
      alu_mod_u:       goto *labels[bin_op<std::modulus<>, std::uint64_t>(++ip)];
      alu_mod_s:       goto *labels[bin_op<std::modulus<>, std::int64_t>(++ip)];
      alu_and:         goto *labels[bin_op<std::bit_and<>, std::uint64_t>(++ip)];
      alu_or:          goto *labels[bin_op<std::bit_or<>, std::uint64_t>(++ip)];
      alu_xor:         goto *labels[bin_op<std::bit_xor<>, std::uint64_t>(++ip)];
      alu_not:         goto *labels[un_op<std::bit_not<>, std::uint64_t>(++ip)];
      alu_shl:         goto *labels[bin_op<bit_shift_left, std::uint64_t>(++ip)];
      alu_shr:         goto *labels[bin_op<bit_shift_right, std::uint64_t>(++ip)];
      alu_sar:         goto *labels[bin_op<bit_shift_right, std::int64_t>(++ip)];

      ctrl_jmp:        goto *labels[jmp(++ip)];
      ctrl_jmpo:       goto *labels[jmpo(++ip)];
      ctrl_jz:         goto *labels[jump_zero<std::equal_to<>, std::uint64_t>(++ip)];
      ctrl_jnz:        goto *labels[jump_zero<std::not_equal_to<>, std::uint64_t>(++ip)];
      ctrl_je:         goto *labels[jump<std::equal_to<>, std::uint64_t>(++ip)];
      ctrl_jne:        goto *labels[jump<std::not_equal_to<>, std::uint64_t>(++ip)];
      ctrl_jl:         goto *labels[jump<std::less<>, std::int64_t>(++ip)];
      ctrl_jle:        goto *labels[jump<std::less_equal<>, std::int64_t>(++ip)];
      ctrl_jb:         goto *labels[jump<std::less<>, std::uint64_t>(++ip)];
      ctrl_jbe:        goto *labels[jump<std::less_equal<>, std::uint64_t>(++ip)];
      ctrl_je_f32:     goto *labels[jump<std::equal_to<>, float>(++ip)];
      ctrl_jne_f32:    goto *labels[jump<std::not_equal_to<>, float>(++ip)];
      ctrl_jl_f32:     goto *labels[jump<std::less<>, float>(++ip)];
      ctrl_jle_f32:    goto *labels[jump<std::less_equal<>, float>(++ip)];
      ctrl_je_f64:     goto *labels[jump<std::equal_to<>, double>(++ip)];
      ctrl_jne_f64:    goto *labels[jump<std::not_equal_to<>, double>(++ip)];
      ctrl_jl_f64:     goto *labels[jump<std::less<>, double>(++ip)];
      ctrl_jle_f64:    goto *labels[jump<std::less_equal<>, double>(++ip)];
      ctrl_inc_jne:    goto *labels[inc_jne<std::uint64_t>(++ip)];
      ctrl_dec_jnz:    goto *labels[dec_jnz<std::uint64_t>(++ip)];
      ctrl_call:       goto *labels[call(++ip)];
      ctrl_ret:        goto *labels[ret(ip)];

      debug_printg:    goto *labels[dprint(++ip)];
      debug_prints:    goto *labels[sprint(++ip)];
      debug_printgs:   goto *labels[dprints(++ip)];
      debug_dumpstk:   goto *labels[dumpstk(++ip)];
      debug_dumpmem:   goto *labels[dumpmem(++ip)];

      sys_abort:
        code = static_cast<VMErrorType>(reg(*++ip));

      sys_exit:
        return code;
    }

    private:
      template <typename Op, typename T>
      [[nodiscard]] std::uint8_t jump(const Instruction*& ip) {
        const auto& A = reg(*ip);
        const auto& B = reg(*++ip);
        const auto offset = fetch_imm<std::int32_t>(++ip);
        if (Op{}(extract_from_reg<T>(A), extract_from_reg<T>(B)))
          ip += offset;
        return *ip;
      }

      template <typename Op, typename T>
      [[nodiscard]] std::uint8_t jump_zero(const Instruction*& ip) {
        const auto& A = reg(*ip);
        const auto offset = fetch_imm<std::int32_t>(++ip);
        if (Op{}(extract_from_reg<T>(A), static_cast<T>(0)))
          ip += offset;
        return *ip;
      }

      [[nodiscard]] std::uint8_t jmp(const Instruction*& ip) {
        const auto offset = fetch_imm<std::int32_t>(ip);
        ip += offset;
        return *ip;
      }

      [[nodiscard]] std::uint8_t jmpo(const Instruction*& ip) {
        const auto& A = reg(*ip);
        ip += extract_from_reg<std::int32_t>(A);
        return *++ip;
      }

      template <typename T>
      [[nodiscard]] std::uint8_t inc_jne(const Instruction*& ip) {
        auto& A = reg(*ip);
        const auto& B = reg(*++ip);
        const auto offset = fetch_imm<std::int32_t>(++ip);
        A = store_to_reg(extract_from_reg<T>(A) + 1);
        if (extract_from_reg<T>(A) != extract_from_reg<T>(B))
          ip += offset;
        return *ip;
      }

      template <typename T>
      [[nodiscard]] std::uint8_t dec_jnz(const Instruction*& ip) {
        auto& A = reg(*ip);
        const auto offset = fetch_imm<std::int32_t>(++ip);
        A = store_to_reg(extract_from_reg<T>(A) - 1);
        if (extract_from_reg<T>(A) != static_cast<T>(0))
          ip += offset;
        return *ip;
      }

      [[nodiscard]] std::uint8_t call(const Instruction*& ip) {
        const auto offset = fetch_imm<std::int32_t>(ip);
        const auto shift = fetch_imm<std::uint32_t>(ip);
        call_stack.push_back(CallFrame{
          .ret_ip = ip,
          .ret_rb = rb
        });
        rb += shift;
        return *(ip += offset);
      }

      [[nodiscard]] std::uint8_t ret(const Instruction*& ip) {
        const auto& frame = call_stack.back();
        ip = frame.ret_ip;
        rb = frame.ret_rb;
        call_stack.pop_back();
        return *ip;
      }

    private:
      template <typename T1, typename T2>
      [[nodiscard]] std::uint8_t cvt(const Instruction*& ip) {
        auto& A = reg(*ip);
        auto& B = reg(*++ip);
        T1 val = extract_from_reg<T1>(B);
        A = store_to_reg(static_cast<T2>(val));
        return *++ip;
      }

      template <typename Op, typename T>
      [[nodiscard]] std::uint8_t un_op(const Instruction*& ip) {
        auto& A = reg(*ip);
        const auto& B = reg(*++ip);
        A = store_to_reg(Op{}(extract_from_reg<T>(B)));
        return *++ip;
      }

      template <typename Op, typename T>
      [[nodiscard]] std::uint8_t bin_op(const Instruction*& ip) {
        auto& A = reg(*ip);
        const auto& B = reg(*++ip);
        const auto& C = reg(*++ip);
        A = store_to_reg(Op{}(extract_from_reg<T>(B), extract_from_reg<T>(C)));
        return *++ip;
      }

    private:
      template <typename T>
      [[nodiscard]] std::uint8_t move(const Instruction*& ip) noexcept {
        auto& A = reg(*ip);
        const auto& B = reg(*++ip);
        A = store_to_reg(static_cast<T>(B));
        return *++ip;
      }

      [[nodiscard]] std::uint8_t swap(const Instruction*& ip) noexcept {
        auto& A = reg(*ip);
        auto& B = reg(*++ip);
        std::swap(A, B);
        return *++ip;
      }

      template <typename T>
      [[nodiscard]] std::uint8_t reg_load(const Instruction*& ip) {
        auto& A = reg(*ip);
        A = store_to_reg(fetch_imm<T>(++ip));
        return *ip;
      }

    private:
      [[nodiscard]] std::uint8_t mem_alloc(const Instruction*& ip) {
        auto& A = reg(*ip);
        const auto& B = reg(*++ip);
        const std::size_t old_size = mem.size();
        mem.resize(old_size + B);
        A = store_to_reg(old_size);
        return *++ip;
      }

      [[nodiscard]] std::uint8_t mem_free(const Instruction*& ip) {
        const auto& A = reg(*ip);
        /*

        */
        return *++ip;
      }

      template <typename T>
      [[nodiscard]] std::uint8_t mem_load(const Instruction*& ip) {
        auto& A = reg(*ip);
        const auto& B = reg(*++ip);
        A = store_to_reg(load_from_mem<T>(B));
        return *++ip;
      }

      template <typename T>
      [[nodiscard]] std::uint8_t mem_store(const Instruction*& ip) {
        const auto& A = reg(*ip);
        const auto& B = reg(*++ip);
        store_from_mem(A, extract_from_reg<T>(B));
        return *++ip;
      }

      [[nodiscard]] std::uint8_t mem_cpy(const Instruction*& ip) {
        const auto& A = reg(*ip);
        const auto& B = reg(*++ip);
        const auto& C = reg(*++ip);
        std::memcpy(&mem[A], &mem[B], C);
        return *++ip;
      }

      [[nodiscard]] std::uint8_t mem_set(const Instruction*& ip) {
        const auto& A = reg(*ip);
        const auto& B = reg(*++ip);
        const auto& C = reg(*++ip);
        std::memset(&mem[A], B, C);
        return *++ip;
      }

      [[nodiscard]] std::uint8_t mem_cmp(const Instruction*& ip) {
        auto& A = reg(*ip);
        const auto& B = reg(*++ip);
        const auto& C = reg(*++ip);
        const auto& D = reg(*++ip);
        A = store_to_reg(std::memcmp(&mem[A], &mem[B], C));
        return *++ip;
      }

    private:
      [[nodiscard]] std::uint8_t stk_alloc(const Instruction*& ip) {
        const auto imm = fetch_imm<std::uint32_t>(ip);
        sb += imm;
        return *ip; 
      }

      [[nodiscard]] std::uint8_t stk_free(const Instruction*& ip) {
        const auto imm = fetch_imm<std::uint32_t>(ip);
        sb -= imm;
        return *ip;
      }

      template <typename T>
      [[nodiscard]] std::uint8_t stk_load(const Instruction*& ip) {
        auto& A = reg(*ip);
        const auto imm = fetch_imm<std::uint32_t>(++ip);
        A = store_to_reg(load_from_stack<T>(imm));
        return *ip;
      }

      template <typename T>
      [[nodiscard]] std::uint8_t stk_store(const Instruction*& ip) {
        const auto& A = reg(*ip);
        const auto imm = fetch_imm<std::uint32_t>(++ip);
        store_from_stack(imm, extract_from_reg<T>(A));
        return *ip;
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
        /*
        
        */
        return *ip;
      }

    private:
      [[nodiscard]] [[gnu::always_inline]] inline Register& reg(const Instruction i) noexcept {
        /*
        
        */
        return regs.get()[rb + i];
      }

      template <typename T>
      [[gnu::always_inline]] inline T fetch_imm(const Instruction*& ip) {
        T imm;
        std::memcpy(&imm, ip, sizeof(T));
        ip += sizeof(T);
        return imm;
      }

      template <typename T>
      [[nodiscard]] [[gnu::always_inline]] inline T load_from_stack(const std::uint32_t offset) const noexcept {
        T val;
        std::memcpy(&val, &stack.get()[sb + offset], sizeof(T));
        return val;
      }

      template <typename T>
      [[gnu::always_inline]] inline void store_from_stack(const std::uint32_t offset, T val) noexcept {
        std::memcpy(&stack.get()[sb + offset], &val, sizeof(T));
      }

      template <typename T>
      [[nodiscard]] [[gnu::always_inline]] inline T load_from_mem(const std::uint32_t offset) const noexcept {
        T val;
        std::memcpy(&val, &mem[offset], sizeof(T));
        return val;
      }

      template <typename T>
      [[gnu::always_inline]] inline void store_from_mem(const std::uint32_t offset, T val) noexcept {
        std::memcpy(&mem[offset], &val, sizeof(T));
      }

      template <typename T>
      [[gnu::always_inline]] inline T extract_from_reg(Register val) const noexcept {
        if constexpr (std::is_floating_point_v<T>) {
          using FT = std::conditional_t<sizeof(T) == 4, std::uint32_t, std::uint64_t>;
          return std::bit_cast<T>(static_cast<FT>(val));
        } else {
          return static_cast<T>(val);
        }
      }

      template <typename T>
      [[gnu::always_inline]] inline std::uint64_t store_to_reg(T val) const noexcept {
        if constexpr (std::is_floating_point_v<T>) {
          using UT = std::conditional_t<sizeof(T) == 4, std::uint32_t, std::uint64_t>;
          return static_cast<std::uint64_t>(std::bit_cast<UT>(val));
        } else {
          return static_cast<std::uint64_t>(val);
        }
      }

    private:
      VMErrorType code = VMErrorType::VMET_NONE;
      std::size_t sb = 0;
      std::size_t rb = 0;
      std::unique_ptr<Register[]> regs;
      std::unique_ptr<std::uint8_t[]> stack;
      std::vector<std::uint8_t>& mem;
      std::vector<CallFrame> call_stack;
  };

}