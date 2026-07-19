export module tona.heap;

import std;

import tona.error;

export namespace Tona {

  class Heap {
    private:
      static constexpr std::size_t page_bytes = 4096;
      static constexpr std::size_t page_free = static_cast<std::size_t>(-1);
      static constexpr std::size_t page_large_head = static_cast<std::size_t>(-2);
      static constexpr std::size_t page_large_body = static_cast<std::size_t>(-3);

      [[nodiscard]] static constexpr std::size_t slots_per_page(std::size_t cls) noexcept {
        return page_bytes / heap_cls[cls];
      }

      static constexpr std::array<std::uint16_t, 15> heap_cls {
        8, 16, 24, 
        32, 48, 64, 
        80, 96, 112, 
        128, 192, 224, 
        256, 384, 512
      };

      struct Page {
        std::size_t pidx;
        std::size_t slots_free;
        std::size_t bump_next;
        std::vector<std::size_t> free_check;
      };

      struct PageMeta {
        std::size_t cls = page_free;
        std::list<Page>::iterator it{};
        bool is_full = false;
        std::size_t span_pages = 0;
      };

    public:
      Heap(std::vector<std::uint8_t>& mem, std::size_t hb) 
        : mem(mem),
          hb(hb)
        {}
      Heap(const Heap&) = delete;
      Heap& operator=(const Heap&) = delete;
      Heap(Heap&&) = delete;
      Heap& operator=(Heap&&) = delete;

      [[nodiscard]] std::size_t alloc(std::size_t alloc_size) {
        alloc_size = std::max<std::size_t>(alloc_size, 1);
        if (alloc_size <= heap_cls.back()) [[likely]]
          return alloc_small(alloc_size);
        return alloc_large(alloc_size);
      }

      [[nodiscard]] VMErrorType free(const std::size_t ptr) {
        if (ptr < hb)
          return VMErrorType::VMET_CONSTANT_FREE;
        const std::size_t pidx = (ptr - hb) / page_bytes;
        if (pidx >= page_meta.size())
          return VMErrorType::VMET_UNKNOW_FREE;
        auto& meta = page_meta[pidx];
        if (meta.cls == page_free)
          return VMErrorType::VMET_DOUBLE_FREE;
        if (meta.cls == page_large_body)
          return VMErrorType::VMET_UNKNOW_FREE;
        if (meta.cls == page_large_head) {
          free_large(pidx, meta.span_pages);
          return VMErrorType::VMET_NONE;
        }
        const std::size_t page_start = hb + pidx * page_bytes;
        if ((ptr - page_start) % heap_cls[meta.cls] != 0)
          return VMErrorType::VMET_UNKNOW_FREE;
        free_small(ptr, pidx, meta);
        return VMErrorType::VMET_NONE;
      }

      [[nodiscard]] const auto data() const noexcept {
        return mem.data();
      }

      template <typename T>
      [[nodiscard]] [[gnu::always_inline]] inline T load(const std::uint32_t offset) const noexcept {
        T val;
        std::memcpy(&val, &mem[offset], sizeof(T));
        return val;
      }

      template <typename T>
      [[gnu::always_inline]] inline void store(const std::uint32_t offset, T val) noexcept {
        std::memcpy(&mem[offset], &val, sizeof(T));
      }

      [[gnu::always_inline]] inline void mem_cpy(std::size_t A, std::size_t B, std::size_t C) noexcept {
        std::memcpy(&mem[A], &mem[B], C);
      }

      [[gnu::always_inline]] inline void mem_set(std::size_t A, std::size_t B, std::size_t C) noexcept {
        std::memset(&mem[A], B, C);
      }

      [[nodiscard]] [[gnu::always_inline]] inline int mem_cmp(std::size_t A, std::size_t B, std::size_t C) noexcept {
        return std::memcmp(&mem[A], &mem.data()[B], C);
      }

    private:
         [[nodiscard]] std::size_t alloc_small(std::size_t alloc_size) {
        const std::size_t cls = std::lower_bound(heap_cls.begin(), heap_cls.end(), alloc_size) - heap_cls.begin();
        auto& partial = partial_pages[cls];

        if (partial.empty()) {
          const std::size_t pidx = acquire_page();
          partial.push_front(Page{
            .pidx        = pidx,
            .slots_free  = slots_per_page(cls),
            .bump_next   = hb + pidx * page_bytes,
          });
          page_meta[pidx] = PageMeta{ 
            .cls = cls, 
            .it = partial.begin(), 
            .is_full = false 
          };
        }

        auto it = partial.begin();
        Page& page = *it;

        std::size_t offset;
        if (!page.free_check.empty()) {
          offset = page.free_check.back();
          page.free_check.pop_back();
        } else {
          offset = page.bump_next;
          page.bump_next += heap_cls[cls];
        }
        page.slots_free--;

        if (page.slots_free == 0) {
          const std::size_t pidx = page.pidx;
          full_pages[cls].splice(full_pages[cls].begin(), partial, it);
          page_meta[pidx].is_full = true;
        }

        return offset;
      }

      void free_small(std::size_t ptr, std::size_t pidx, PageMeta& meta) {
        Page& page = *meta.it;
        page.free_check.push_back(ptr);
        const bool was_full = meta.is_full;
        page.slots_free++;

        if (page.slots_free == slots_per_page(meta.cls)) {
          auto& lst = was_full ? full_pages[meta.cls] : partial_pages[meta.cls];
          lst.erase(meta.it);
          meta.cls = page_free;
          free_pages.push_back(pidx);
          return;
        }

        if (was_full) {
          partial_pages[meta.cls].splice(partial_pages[meta.cls].begin(), full_pages[meta.cls], meta.it);
          meta.is_full = false;
        }
      }

      [[nodiscard]] std::size_t alloc_large(std::size_t alloc_size) {
        const std::size_t span_pages = (alloc_size + page_bytes - 1) / page_bytes;
        const std::size_t first_pidx = page_count;

        mem.resize(hb + (first_pidx + span_pages) * page_bytes);
        page_count += span_pages;
        page_meta.resize(page_count);

        page_meta[first_pidx] = PageMeta{ 
          .cls = page_large_head, 
          .span_pages = span_pages 
        };

        for (std::size_t i = 1; i < span_pages; ++i) {
          page_meta[first_pidx + i] = PageMeta{ 
            .cls = page_large_body 
          };
        }

        return hb + first_pidx * page_bytes;
      }

      void free_large(std::size_t pidx, std::size_t span_pages) {
        for (std::size_t i = 0; i < span_pages; ++i) {
          page_meta[pidx + i].cls = page_free;
          free_pages.push_back(pidx + i);
        }
      }

    private:
      [[nodiscard]] std::size_t acquire_page() {
        if (!free_pages.empty()) {
          const std::size_t pidx = free_pages.back();
          free_pages.pop_back();
          return pidx;
        }
        const std::size_t pidx = page_count;
        mem.resize(hb + (pidx + 1) * page_bytes);
        page_count++;
        page_meta.resize(page_count);
        return pidx;
      }

    private:
      std::vector<std::uint8_t>& mem;
      std::size_t hb;
      std::size_t page_count = 0;
      std::vector<std::size_t> free_pages;
      std::vector<PageMeta> page_meta;
      std::array<std::list<Page>, heap_cls.size()> partial_pages;
      std::array<std::list<Page>, heap_cls.size()> full_pages;
  };

}