export module tona.type_ctx;

import std;

export namespace Tona {

  class TypeContext {
    public:
      TypeContext() {
        
      }

      TypeContext(const TypeContext&) = delete;
      TypeContext& operator=(const TypeContext&) = delete;
      TypeContext(TypeContext&&) = delete;
      TypeContext& operator=(TypeContext&&) = delete;

    private:
  };

}