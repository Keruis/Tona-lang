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
  auto i = ass.compile(R"(
    load8 r0, 10
    load8 r1, 5
    load8 r2, 1
    load8 r3, 0

  loop:
    add r4, r0, r4
    sub r1, r1, r2
    jne r1, r3 loop
    printg r4
    end
  )");

  vm.run(i.data());

  
}