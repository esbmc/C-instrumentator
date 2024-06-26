#include <catch2/catch_test_macros.hpp>
#include <bounded-loop-unroller/bounded-loop-unroller.h>
// While
TEST_CASE("One-line empty For-Loops are removed", "[for]") {
    auto initial = R"(
int main() { for (int i = 0; i < 3; i++) ; return 0; })";
    auto expected = R"(
int main() {  return 0; })";
    auto actual = BoundedLoopUnroller::RunLoopUnroller(initial, "out.c");
    REQUIRE(actual == expected);
}

TEST_CASE("Multi-line empty For-Loops are removed", "[for]") {
    auto initial = R"(
int main() { for (int i = 0; i < 3; i++) {

}
return 0; })";
    auto expected = R"(
int main() { 
return 0; })";
    auto actual = BoundedLoopUnroller::RunLoopUnroller(initial, "out.c");
    REQUIRE(actual == expected);
}

# if 0

TEST_CASE("One-line contradictory For-Loops are removed", "[for]") {
    auto initial = R"(
int main() { for (int i = 0; i > 3; i++) break; return 0; })";
    auto expected = R"(
int main() {  return 0; })";
    auto actual = BoundedLoopUnroller::RunLoopUnroller(initial, "out.c");
    REQUIRE(actual == expected);
}

TEST_CASE("Multi-line  contradictory For-Loops are removed", "[for]") {
    auto initial = R"(
int main() { for (int i = 0; i > 3; i++) {
break;
}
return 0; })";
    auto expected = R"(
int main() { 
return 0; })";
    auto actual = BoundedLoopUnroller::RunLoopUnroller(initial, "out.c");
    REQUIRE(actual == expected);
}

#endif
