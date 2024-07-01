#include <__format/format_functions.h>
#include <bounded-loop-unroller/bounded-loop-unroller.h>
#include <cstdlib>
#include <fstream>
#include <iostream>

#include <algorithm>
#include <iterator>
#include <string>


int main(int argc, char **argv) {
  if (argc != 3) {
      std::cerr << "./loop-unroller <input.c> <output.c>\n";
      abort();
    }

  BoundedLoopUnroller::Run(argv[1], argv[2]);
  return 0;
}
