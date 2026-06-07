export module tona.buf;

import std;

import tona.config;
import tona.types;

export namespace Tona {

  class TBuf {
    public:
      TBuf() : cap(TBUF_INIT_SIZE) {
        buf = new char[TBUF_INIT_SIZE];
      }
      ~TBuf() {
        delete[] buf;
      }

      void stuff_back(char c) {
        reserve(size + 1);
        buf[size++] = c;
      }

      void stuff_back(const char* src, std::size_t len) {
        reserve(size + len);
        std::memcpy(buf + size, src, len);
      }

      std::size_t reset() {
        std::size_t old_size = size;
        size = 0;
        return old_size;
      }

      t_rbytes buffer() {
        return buf;
      }

    private:
      void reserve(std::size_t min_size) {
        if (min_size > cap) {
          std::size_t alloc_size = min_size * TBUF_GROWTH_FACTOR;
          t_rbytes old_ptr = buf;
          buf = new t_byte[alloc_size];
          cap = alloc_size;
          std::memcpy(buf, old_ptr, size);
          delete[] old_ptr;
        }
      }

    private:
      t_bytes buf;
      std::size_t size;
      std::size_t cap;
  };
}