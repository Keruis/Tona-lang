export module tona.arena;

import std;

export namespace Tona {

  class Arena : public std::pmr::memory_resource {
      struct Block {
        std::uint8_t* data;
        std::size_t size = 0;
        std::size_t capacity;

        Block(std::size_t cap)
          : capacity(cap)
        {
          data = new std::uint8_t[cap];
        }
        Block(const Block&) = delete;
        Block& operator=(const Block&) = delete;
        Block(Block&&) = delete;
        Block& operator=(Block&&) = delete;
        ~Block() {
          delete[] data;
        }
      };

      struct CleanupNode {
        void (*destroy)(void*);
        void* obj;
        CleanupNode* next;
      };

    public:
      Arena(const Arena&) = delete;
      Arena& operator=(const Arena&) = delete;
      Arena(Arena&&) = delete;
      Arena& operator=(Arena&&) = delete;
      explicit Arena(std::size_t default_capacity) 
        : default_capacity(default_capacity) 
      {
        blocks.emplace_back(default_capacity);
        cur_block = blocks.begin();
      }
      ~Arena() {
        reset();
      }

      [[nodiscard]] void* allocate(std::size_t size, std::size_t alignment) {
        void* ptr = cur_block->data + cur_block->size;
        void* aligned_ptr = ptr;
        std::size_t space = cur_block->capacity - cur_block->size;
        if (!std::align(alignment, size, aligned_ptr, space)) [[unlikely]] {
          advance_block(size, alignment);
          ptr = cur_block->data;
          aligned_ptr = ptr;
          space = cur_block->capacity;
        }

        cur_block->size += (std::bit_cast<std::uintptr_t>(aligned_ptr) - std::bit_cast<std::uintptr_t>(ptr)) + size;
        return aligned_ptr;
      }

      template <typename T, typename... Args>
      [[nodiscard]] T* create(Args&&... args) {
        void* mem = allocate(sizeof(T), alignof(T));
        T* obj = std::construct_at(static_cast<T*>(mem), std::forward<Args>(args)...);
        if constexpr (!std::is_trivially_destructible_v<T>)
          register_cleanup(obj, [](void* obj){
            static_cast<T*>(obj)->~T();
          });
        return obj;
      }

      void reset() {
        CleanupNode* cur_cleanup = cleanup_list;
        while (cur_cleanup != nullptr) {
          cur_cleanup->destroy(cur_cleanup->obj);
          cur_cleanup = cur_cleanup->next;
        }
        cleanup_list = nullptr;

        for (auto& block : blocks)
          block.size = 0;
        cur_block = blocks.begin();
      }

    protected:
      void* do_allocate(std::size_t bytes, std::size_t alignment) override {
        return allocate(bytes, alignment);
      }

      void do_deallocate(void*, std::size_t, std::size_t) override {
        
      }

      bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override {
        return this == &other;
      }

    private:
      void register_cleanup(void* obj, void (*destroy)(void*)) {
        void* mem = allocate(sizeof(CleanupNode), alignof(CleanupNode));
        CleanupNode* node = std::construct_at(static_cast<CleanupNode*>(mem), destroy, obj, cleanup_list);
        cleanup_list = node;
      }

      void advance_block(std::size_t size, std::size_t alignment) {
        auto next = std::next(cur_block);
        const std::size_t required = size + alignment - 1;

        if (next != blocks.end() && next->capacity >= required) {
          cur_block = next;
          return;
        }

        const std::size_t new_capacity = std::max(default_capacity, required);
        cur_block = blocks.emplace(next, new_capacity);
      }

    private:
      std::size_t default_capacity;
      std::list<Block> blocks;
      std::list<Block>::iterator cur_block;
      CleanupNode* cleanup_list = nullptr;
  };

}