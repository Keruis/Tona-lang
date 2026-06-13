module;

#include <cassert>

export module tona.assembler;

import std;

import tona.opcode;
import tona.byte;

export namespace Tona {

  class Assembler {
    public:
      std::vector<Instruction> compile(std::string_view text) {
        std::vector<Instruction> bytes;

        for (auto&& line_range : text | std::views::split('\n')) {
          std::string_view line(line_range);
          std::string_view cleaned = trim(line);
          if (cleaned.empty())
            continue;

          auto vec_tok = tokenize(cleaned);
          if (vec_tok.empty())
            continue;

          emit_insturction(vec_tok, bytes);
        }

        return bytes;
      }

    private:
      [[nodiscard]] std::string_view trim(std::string_view line) {
        auto start = line.find_first_not_of(" \n\t\r");
        if (start == std::string_view::npos)
          return {};
        auto end = line.find_last_not_of(" \n\t\r");
        return line.substr(start, end - start + 1);
      }

      std::vector<std::string_view> tokenize(std::string_view line) {
        auto is = [](char c){
          return c == ' ' || c == '\t' || c == ',';
        };

        return line | std::views::chunk_by([&](char a, char b){    
          return is(a) == is(b);
        }) | std::views::filter([&](auto&& chunk){
          return !is(chunk.front());
        }) | std::views::transform([&](auto&& chunk){
          return std::string_view(chunk);
        }) | std::ranges::to<std::vector<std::string_view>>();
      }

      std::uint8_t parse_reg(std::string_view token) {
        std::uint8_t id;
        auto [_, ec] = std::from_chars(token.begin() + 1, token.end(), id);
        assert(ec == std::errc{});
        return id;
      }

      template <typename T>
      void push_val(std::vector<Instruction>& bytes, T val) {
        std::size_t old_size = bytes.size();
        bytes.resize(old_size + sizeof(T));
        std::memcpy(bytes.data() + old_size, &val, sizeof(T));
      }

      void emit_insturction(std::span<std::string_view> vec_tok, std::vector<Instruction>& bytes) {
        switch (auto op = find_opcode(vec_tok[0])) {
          case OpCode::OC_END:
            bytes.push_back(cast_u8(op));
            break;
          case OpCode::OC_MOVE:
          case OpCode::OC_FMOVE:
            bytes.push_back(cast_u8(op));
            bytes.push_back(parse_reg(vec_tok[1]));
            bytes.push_back(parse_reg(vec_tok[2]));
            break;
          case OpCode::OC_LOAD8:
          case OpCode::OC_LOAD16:
          case OpCode::OC_LOAD32:
          case OpCode::OC_LOAD: {
            std::uint8_t id = parse_reg(vec_tok[1]);
            std::uint64_t val;
            auto [_, ec] = std::from_chars(vec_tok[2].begin(), vec_tok[2].end(), val);
            assert(ec == std::errc{});
            bytes.push_back(cast_u8(op));
            bytes.push_back(id);
            switch (op) {
              case OpCode::OC_LOAD8:
                push_val(bytes, static_cast<std::uint8_t>(val));
                break;
              case OpCode::OC_LOAD16:
                push_val(bytes, static_cast<std::uint16_t>(val));
                break;
              case OpCode::OC_LOAD32:
                push_val(bytes, static_cast<std::uint32_t>(val));
                break;
              case OpCode::OC_LOAD:
                push_val(bytes, val);
                break;
              default:
                std::unreachable();
            }
            break;
          }
          case OpCode::OC_FLOAD32:
          case OpCode::OC_FLOAD: {
            std::uint8_t id = parse_reg(vec_tok[1]);
            double val;
            auto [_, ec] = std::from_chars(vec_tok[2].begin(), vec_tok[2].end(), val);
            assert(ec == std::errc{});
            bytes.push_back(cast_u8(op));
            bytes.push_back(id);
            switch(op) {
              case OpCode::OC_FLOAD32:
                push_val(bytes, static_cast<float>(val));
                break;
              case OpCode::OC_FLOAD:
                push_val(bytes, val);
                break;
              default:
                std::unreachable();
            }
            break;
          }
          case OpCode::OC_ADD:
          case OpCode::OC_SUB:
          case OpCode::OC_MUL:
          case OpCode::OC_DIV:
          case OpCode::OC_MOD:
          case OpCode::OC_FADD:
          case OpCode::OC_FSUB:
          case OpCode::OC_FMUL:
          case OpCode::OC_FDIV: {
            bytes.push_back(cast_u8(op));
            bytes.push_back(parse_reg(vec_tok[1]));
            bytes.push_back(parse_reg(vec_tok[2]));
            bytes.push_back(parse_reg(vec_tok[3]));
            break;
          }
          case OpCode::OC_PRINT_G:
          case OpCode::OC_PRINT_F:
            bytes.push_back(cast_u8(op));
            bytes.push_back(parse_reg(vec_tok[1]));
        }
      }
  };

}