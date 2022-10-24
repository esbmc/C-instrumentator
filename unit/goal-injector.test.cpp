#include <catch2/catch_test_macros.hpp>
#include <goal-injector/goal-injector.h>

#ifndef NDEBUG

TEST_CASE("Lined While Loops are instrumented", "[while]") {
    auto initial = "int main() { while(1){} return 0; }";
    auto expected = "int main() { while(1){ reach_error(); } return 0; }";
    auto actual = GoalInjector::RunGoalInjector(initial, "out.c");
    REQUIRE(actual == expected);
}

TEST_CASE("Multi-line While Loops are instrumented", "[while]") {
    auto initial = R"(
int main() {
  while(1) {
  }
  return 0;
})";
    auto expected = R"(
int main() {
  while(1) {
   reach_error(); }
  return 0;
})";

    auto actual = GoalInjector::RunGoalInjector(initial, "out.c");
    REQUIRE(actual == expected);
}

TEST_CASE("Recursive While Loops are instrumented", "[while]") {
    auto initial = R"(
int main() {
  while(1) {
    while(1) {}
  }
  return 0;
})";
    auto expected = R"(
int main() {
  while(1) {
    while(1) { reach_error(); }
   reach_error(); }
  return 0;
})";

    auto actual = GoalInjector::RunGoalInjector(initial, "out.c");
    REQUIRE(actual == expected);
}

#endif