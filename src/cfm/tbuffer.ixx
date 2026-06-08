export module tona.buf;

import std;

import tona.config;
import tona.types;

export namespace Tona {

  class TBuf {
    public:
      TBuf() : cap(TBUF_INIT_SIZE) {
        buf = new t_byte[TBUF_INIT_SIZE];
      }
      ~TBuf() {
        delete[] buf;
      }

      void stuff_back(t_c c) {
        reserve(size + 1);
        buf[size++] = c;
      }

      void stuff_back(t_rcp src, usize len) {
        reserve(size + len);
        std::memcpy(buf + size, src, len);
      }

      usize reset() {
        std::size_t old_size = size;
        size = 0;
        return old_size;
      }

      template <typename RT>
      RT buffer() {
        return reinterpret_cast<RT>(buf);
      }

    private:
      void reserve(usize min_size) {
        if (min_size > cap) {
          usize alloc_size = min_size * TBUF_GROWTH_FACTOR;
          t_rbytes old_ptr = buf;
          buf = new t_byte[alloc_size];
          cap = alloc_size;
          std::memcpy(buf, old_ptr, size);
          delete[] old_ptr;
        }
      }

    private:
      t_bytes buf;
      usize size;
      usize cap;
  };
}