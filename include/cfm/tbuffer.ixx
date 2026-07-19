export module tona.buf;

import std;

import tona.config;

export namespace Tona {

  class Buf {
    public:
      Buf() {}
      Buf(const Buf&) = delete;
      Buf& operator=(const Buf&) = delete;
      Buf(Buf&& other) noexcept 
        : buf(std::exchange(other.buf, nullptr)),
          size(std::exchange(other.size, 0)),
          cap(std::exchange(other.cap, 0)) {}
      Buf& operator=(Buf&& other) noexcept {
        if (this != &other) {
          delete[] buf;
          buf = std::exchange(other.buf, nullptr);
          size = std::exchange(other.size, 0);
          cap = std::exchange(other.cap, 0);
        }
        return *this;
      }
      ~Buf() {
        delete[] buf;
      }

      void stuff_set(std::uint8_t b, std::size_t i) {
        reserve(i);
        buf[i] = b;
        size = (size < i ? i : size);
      }

      template <typename T>
        requires(std::is_trivially_copyable_v<T>)
      void stuff_back(const T& obj) {
        reserve(size + sizeof(T));
        std::memcpy(&buf[size], &obj, sizeof(T));
        size += sizeof(T);
      }

      template <typename T>
        requires(std::is_trivially_copyable_v<T>)
      void stuff_back(const T* src, std::size_t len) {
        std::size_t alloc_size = len * sizeof(T);
        reserve(size + alloc_size);
        std::memcpy(&buf[size], src, alloc_size);
        size += alloc_size;
      }

      [[nodiscard]] std::size_t buf_size() const noexcept {
        return size;
      }

      [[nodiscard]] std::size_t buf_cap() const noexcept {
        return cap;
      }

      void reset() noexcept {
        size = 0;
      }

      template <typename RT>
      [[nodiscard]] RT buffer() const noexcept {
        return reinterpret_cast<RT>(buf);
      }

    private:
      void reserve(std::size_t min_size) {
        if (min_size > cap) {
          std::size_t alloc_size = min_size * TBUF_GROWTH_FACTOR;
          std::uint8_t* old_ptr = buf;
          buf = new std::uint8_t[alloc_size];
          cap = alloc_size;
          std::memcpy(buf, old_ptr, size);
          delete[] old_ptr;
        }
      }

    private:
      std::uint8_t* buf = new std::uint8_t[TBUF_INIT_SIZE];
      std::size_t size = 0;
      std::size_t cap = TBUF_INIT_SIZE;
  };
}