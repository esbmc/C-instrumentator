#ifndef CINSTRUMENTATION_GOAL_INJECTOR_H
#define CINSTRUMENTATION_GOAL_INJECTOR_H

#include <string>
#include <string_view>
#include <fstream>
namespace BoundedLoopUnroller {
    std::string RunLoopUnroller(const std::string input, const std::string output);
    std::string RunLoopUnroller(const std::ifstream &input,
                                const std::string output);

  void Run(const std::string_view &input, const std::string_view &output);
}
#endif //CINSTRUMENTATION_GOAL_INJECTOR_H
