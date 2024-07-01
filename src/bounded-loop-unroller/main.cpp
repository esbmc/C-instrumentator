#include <bounded-loop-unroller/bounded-loop-unroller.h>
#include <iostream>

int main(int argc, char **argv) {
  if (argc != 3) {
      std::cerr << "./loop-unroller <input.c> <output.c>\n";
      abort();
    }

  BoundedLoopUnroller::Run(argv[1], argv[2]);
  return 0;
}
