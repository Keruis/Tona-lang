export module tona.buf;

import std;

import tona.config;
import tona.types;

export namespace Tona {

  class TBuf {
    public:
      TBuf() : cap(TBUF_INIT_SIZE) {
        buf = new std::byte[TBUF_INIT_SIZE];
      }
      ~TBuf() {
        delete[] buf;
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
        reserve(size + len);
        std::memcpy(&buf[size], src, len * sizeof(T));
        size += (len * sizeof(T));
      }

      std::size_t buf_size() {
        return size;
      }

      void reset() {
        size = 0;
      }

      template <typename RT>
      RT buffer() {
        return reinterpret_cast<RT>(buf);
      }

    private:
      void reserve(std::size_t min_size) {
        if (min_size > cap) {
          std::size_t alloc_size = min_size * TBUF_GROWTH_FACTOR;
          std::byte* old_ptr = buf;
          buf = new std::byte[alloc_size];
          cap = alloc_size;
          std::memcpy(buf, old_ptr, size);
          delete[] old_ptr;
        }
      }

    private:
      std::byte* buf;
      std::size_t size;
      std::size_t cap;
  };
}