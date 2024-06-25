#include <goal-injector/goal-injector.h>
#include <fstream>
#include <iostream>

int main(int argc, char **argv) {
  if (argc != 3) 
    std::cerr << "./injector <input.c> <output.c>\n";
  const std::ifstream input(argv[1]);
  GoalInjector::RunGoalInjectorA(input, argv[2]);
  return 0;
}
