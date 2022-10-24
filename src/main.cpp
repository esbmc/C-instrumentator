#include <goal-injector/goal-injector.h>

int main(int argc, char **argv) {
    if (argc > 1)
        GoalInjector::RunGoalInjector(argv[1], argv[2]);
    return 0;
}