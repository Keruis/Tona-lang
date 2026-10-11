#include "../../../include/tb/basic/Diagnostic.h"

namespace tb::err {

std::string Diagnostic::render() const {
  const std::string number = std::to_string(line);
  std::string out;
  out += "error: " + message + "\n";
  out += " --> " + number + ":" + std::to_string(column) + "\n";
  if (!source_line.empty()) {
    const std::string gutter(number.size(), ' ');
    out += gutter + " |\n";
    out += number + " | " + std::string(source_line) + "\n";
    std::string caret(column > 0 ? column - 1 : 0, ' ');
    out += gutter + " | " + caret + "^\n";
  }
  return out;
}

}