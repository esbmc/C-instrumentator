#include <goal-injector/goal-injector.h>
#include <fstream>

int main(int argc, char **argv) {
    if (argc > 2)
    {
        const std::ifstream input(argv[1]);
        GoalInjector::RunGoalInjectorA(input, argv[2]);
    }
    return 0;
}
