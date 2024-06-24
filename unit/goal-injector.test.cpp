#include <catch2/catch_test_macros.hpp>
#include <goal-injector/goal-injector.h>
// While
TEST_CASE("Lined While Loops are instrumented", "[while]") {
    auto initial = "int main() { while(1){} return 0; }";
    auto expected = "int main() {__ESBMC_assert(0, \"0\"); while(1){__ESBMC_assert(0, \"1\");} return 0; }";
    auto actual = GoalInjector::RunGoalInjector(initial, "out.c");
    REQUIRE(actual == expected);
}

TEST_CASE("Lined While Loops are instrumented (no scope)", "[while]") {
    auto initial = "int main() { while(1); return 0; }";
    auto expected = "int main() {__ESBMC_assert(0, \"0\"); while(1){__ESBMC_assert(0, \"1\");;} return 0; }";
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
int main() {__ESBMC_assert(0, "0");
  while(1) {__ESBMC_assert(0, "1");
  }
  return 0;
})";

    auto actual = GoalInjector::RunGoalInjector(initial, "out.c");
    REQUIRE(actual == expected);
}

TEST_CASE("Multi-line While Loops are instrumented (no scope)", "[while]") {
    auto initial = R"(
int main() {
  while(1)
    ;
  return 0;
})";
    auto expected = R"(
int main() {__ESBMC_assert(0, "0");
  while(1)
    {__ESBMC_assert(0, "1");;}
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
int main() {__ESBMC_assert(0, "0");
  while(1) {__ESBMC_assert(0, "1");
    while(1) {__ESBMC_assert(0, "2");}
  }
  return 0;
})";

    auto actual = GoalInjector::RunGoalInjector(initial, "out.c");
    REQUIRE(actual == expected);
}

TEST_CASE("Recursive While Loops are instrumented (no-scope)", "[while]") {
    auto initial = R"(
int main() {
  while(1)
    while(1) ;
  return 0;
})";
    auto expected = R"(
int main() {__ESBMC_assert(0, "0");
  while(1)
    {__ESBMC_assert(0, "1");while(1) {__ESBMC_assert(0, "2");;}}
  return 0;
})";

    auto actual = GoalInjector::RunGoalInjector(initial, "out.c");
    REQUIRE(actual == expected);
}

// IF-Else
TEST_CASE("Lined If is instrumented", "[if]") {
    auto initial = "int main() { if(1){} return 0; }";
    auto expected = "int main() {__ESBMC_assert(0, \"0\"); if(1){__ESBMC_assert(0, \"1\");} return 0; }";
    auto actual = GoalInjector::RunGoalInjector(initial, "out.c");
    REQUIRE(actual == expected);
}

TEST_CASE("Lined If-Else is instrumented", "[if]") {
    auto initial = "int main() { if(1){} else{} return 0; }";
    auto expected = "int main() {__ESBMC_assert(0, \"0\"); if(1){__ESBMC_assert(0, \"1\");} else{__ESBMC_assert(0, \"2\");} return 0; }";
    auto actual = GoalInjector::RunGoalInjector(initial, "out.c");
    REQUIRE(actual == expected);
}

TEST_CASE("Nested If is instrumented", "[if]") {
    auto initial = "int main() { if(1) if(1){} return 0; }";
    auto expected = "int main() {__ESBMC_assert(0, \"0\"); if(1) {__ESBMC_assert(0, \"1\");if(1){__ESBMC_assert(0, \"2\");}} return 0; }";
    auto actual = GoalInjector::RunGoalInjector(initial, "out.c");
    REQUIRE(actual == expected);
}

// FOR
TEST_CASE("Lined For is instrumented", "[for]") {
    auto initial = "int main() { for(;;){} return 0; }";
    auto expected = "int main() {__ESBMC_assert(0, \"0\"); for(;;){__ESBMC_assert(0, \"1\");} return 0; }";
    auto actual = GoalInjector::RunGoalInjector(initial, "out.c");
    REQUIRE(actual == expected);
}

TEST_CASE("Nested For is instrumented", "[for]") {
    auto initial = "int main() { for(;;) for(;;){} return 0; }";
    auto expected = "int main() {__ESBMC_assert(0, \"0\"); for(;;) {__ESBMC_assert(0, \"1\");for(;;){__ESBMC_assert(0, \"2\");}} return 0; }";
    auto actual = GoalInjector::RunGoalInjector(initial, "out.c");
    REQUIRE(actual == expected);
}

// Function
TEST_CASE("Function is instrumented", "[function]") {
    auto initial = "int foo() {return 0;} int bar() {return 1;}";
    auto expected = "int foo() {__ESBMC_assert(0, \"0\");return 0;} int bar() {__ESBMC_assert(0, \"1\");return 1;}";
    auto actual = GoalInjector::RunGoalInjector(initial, "out.c");
    REQUIRE(actual == expected);
}
TEST_CASE("Non-defined Function is (not) instrumented", "[function]") {
    auto initial = "int foo() {return 0;} int bar();";
    auto expected = "int foo() {__ESBMC_assert(0, \"0\");return 0;} int bar();";
    auto actual = GoalInjector::RunGoalInjector(initial, "out.c");
    REQUIRE(actual == expected);
}
// Switch-Case
