import std;

import tona.elysia;

import tona.vm;
import tona.assembler;

int main(int argc, char* argv[]) {
  // std::ofstream out_file;
  // std::ostream* output_stream = &std::cout;
  // out_file.open("test.txt");
  // if (!out_file.is_open()) {
  //   return -1;
  // }
  // output_stream = &out_file;

  // Tona::Elysia driver(*output_stream);
  // return driver.do_file(argv[1]);

  Tona::VM vm;
  Tona::Assembler ass;
  auto is = ass.compile(R"(
    load8 r1, 10
    load8 r2, 20
    add r3, r1, r2
    printg r3
    end
  )");
  vm.run(is.data());
}