#ifndef CINSTRUMENTATION_GOAL_INJECTOR_H
#define CINSTRUMENTATION_GOAL_INJECTOR_H

#include <string>
#include <fstream>
namespace GoalInjector {
    std::string RunGoalInjector(const std::string input, const std::string output);
    std::string RunGoalInjectorA(const std::ifstream &input, const std::string output);
}
#endif //CINSTRUMENTATION_GOAL_INJECTOR_H
