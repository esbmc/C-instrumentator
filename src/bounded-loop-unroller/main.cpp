#include <bounded-loop-unroller/bounded-loop-unroller.h>
#include <fstream>
#include <iostream>

int main(int argc, char **argv) {
  if (argc != 3) 
    std::cerr << "./loop-unroller <input.c> <output.c>\n";
  const std::ifstream input(argv[1]);
  BoundedLoopUnroller::RunLoopUnroller(input, argv[2]);
  return 0;
}
