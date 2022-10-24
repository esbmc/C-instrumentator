#include <goal-injector/frontend_action.h>

int main(int argc, char **argv) {
    if (argc > 1)
        GoalInjector::RunGoalInjector(argv[1], "output.c");
    return 0;
}