import std;

import tona.elysia;

import tona.vm;
import tona.assembler;

int main(int argc, char* argv[]) {
  std::ofstream out_file;
  std::ostream* output_stream = &std::cout;
  out_file.open("test.txt");
  if (!out_file.is_open()) {
    return -1;
  }
  output_stream = &out_file;

  Tona::Elysia driver(*output_stream);
  return driver.do_file(argv[1]);
  // auto mem = std::vector<std::uint8_t>();
  // mem.push_back(5);
  // mem.push_back(7);
  // mem.push_back(10);
  // mem.push_back(90);
  // mem.push_back(8);
  // mem.push_back(5);
   
  // //Tona::Assembler ass;
  // // auto i = ass.compile(mem, R"(
  // //   start:
  // //     load8 r0, 0
  // //     load8 r1, 5
  // //     ldm8 r1, r1
  // //     move r2, r0
  // //     move r3, r1
  // //     call max_val, 2
  // //     printg r2
  // //     ret

  // //   max_val:
  // //     ldm8 r2, r0
  // //     load8 r3, 0
  // //   loop_start:
  // //     dec r1, r1
  // //     je r1, r3, loop_end
  // //     inc r0, r0
  // //     ldm8 r4, r0
  // //     jle r4, r2, loop_start
  // //     move r2, r4
  // //     jmp loop_start

  // //   loop_end:
  // //     move r0, r2
  // //     ret
  // // )");
  // Tona::VM vm(mem);
  // //vm.run(i.data());

  
}