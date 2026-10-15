#ifndef __LEXDIAGNOSTIC__H__
#define __LEXDIAGNOSTIC__H__

#include "Diagnostic.h"

#include <stdexcept>

namespace tb {

class LexError : public std::runtime_error {
  public:
    explicit LexError(Diagnostic&& diag)
      : std::runtime_error(diag.message), diag(std::move(diag))
    {}

    const auto& diagnostic() const {return diag;}

  private:
    Diagnostic diag;
};

}

#endif