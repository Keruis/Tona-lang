export module tona.stack;

import std;

export namespace Tona {

  class Stack {
    public:
      Stack(const std::size_t& stk_base, const std::size_t stk_size) 
        : sb(stk_base),
          stack(std::make_unique<std::uint8_t[]>(stk_size))
      {}
      Stack(const Stack&) = delete;
      Stack& operator=(const Stack&) = delete;
      Stack(Stack&&) = delete;
      Stack& operator=(Stack&&) = delete;

      template <typename T>
      [[nodiscard]] [[gnu::always_inline]] inline T load(const std::uint32_t offset) const noexcept {
        T val;
        std::memcpy(&val, &stack.get()[sb - offset], sizeof(T));
        return val;
      }

      template <typename T>
      [[gnu::always_inline]] inline void store(const std::uint32_t offset, T val) noexcept {
        std::memcpy(&stack.get()[sb - offset], &val, sizeof(T));
      }

    private:
      const std::size_t& sb;
      std::unique_ptr<std::uint8_t[]> stack;
  };

}