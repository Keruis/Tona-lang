import std;

import tona.elysia;

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
}