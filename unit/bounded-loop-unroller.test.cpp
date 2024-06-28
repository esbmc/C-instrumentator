#include <catch2/catch_test_macros.hpp>
#include <bounded-loop-unroller/bounded-loop-unroller.h>


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

TEST_CASE("One-line For-Loops are detected", "[for]") {
    auto initial = R"(
int main() { int a; for (int i = 0; i < 3; i++) a++; return 0; })";
    auto expected = R"(
int main() { int a; 
// LOOP 0 START. ITERATIONS: 3. INIT: int i = 0;. INCR: i++;.
for (int i = 0; i < 3; i++) 
// LOOP 0 BODY START
a++;
// LOOP 0 BODY END
// LOOP 0 END
 return 0; })";
    auto actual = BoundedLoopUnroller::RunLoopUnroller(initial, "out.c");
    REQUIRE(actual == expected);
}

TEST_CASE("Multi-line For-Loops are detected", "[for]") {
    auto initial = R"(
int main() {
int a;
for (int i = 1; i < 5; i++) {
a++;
}
return 0; })";
    auto expected = R"(
int main() {
int a;

// LOOP 0 START. ITERATIONS: 4. INIT: int i = 1;. INCR: i++;.
for (int i = 1; i < 5; i++) {

// LOOP 0 BODY START
a++;

// LOOP 0 BODY END
}
// LOOP 0 END

return 0; })";
    auto actual = BoundedLoopUnroller::RunLoopUnroller(initial, "out.c");
    REQUIRE(actual == expected);
}


TEST_CASE("Commented For-Loops are detected", "[for]") {
    auto initial = R"(
int main() {
int a;
/* ASD */
for (int i = 1; i < 5; i++) {
a++;
}
return 0; })";
    auto expected = R"(
int main() {
int a;
/* ASD */

// LOOP 0 START. ITERATIONS: 4. INIT: int i = 1;. INCR: i++;.
for (int i = 1; i < 5; i++) {

// LOOP 0 BODY START
a++;

// LOOP 0 BODY END
}
// LOOP 0 END

return 0; })";
    auto actual = BoundedLoopUnroller::RunLoopUnroller(initial, "out.c");
    REQUIRE(actual == expected);
}

TEST_CASE("Multi-lineCommented For-Loops are detected", "[for]") {
    auto initial = R"(
int main() {
int a;
/*
  ASD
*/
for (int i = 1; i < 5; i++) {
a++;
}
return 0; })";
    auto expected = R"(
int main() {
int a;
/*
  ASD
*/

// LOOP 0 START. ITERATIONS: 4. INIT: int i = 1;. INCR: i++;.
for (int i = 1; i < 5; i++) {

// LOOP 0 BODY START
a++;

// LOOP 0 BODY END
}
// LOOP 0 END

return 0; })";
    auto actual = BoundedLoopUnroller::RunLoopUnroller(initial, "out.c");
    REQUIRE(actual == expected);
}

TEST_CASE("Nested For-Loops are detected", "[for]") {
    auto initial = R"(
int main() { int a; for (int i = 0; i < 3; i++) for(int j = 0; j < 10; j++) a++; return 0; })";
    auto expected = R"(
int main() { int a; 
// LOOP 0 START. ITERATIONS: 3. INIT: int i = 0;. INCR: i++;.
for (int i = 0; i < 3; i++) 
// LOOP 0 BODY START
// LOOP 1 START. ITERATIONS: 10. INIT: int j = 0;. INCR: j++;.
for(int j = 0; j < 10; j++)  
// LOOP 1 BODY START
a++;
// LOOP 1 BODY END
// LOOP 1 END
// LOOP 0 BODY END
// LOOP 0 END
 return 0; })";
    auto actual = BoundedLoopUnroller::RunLoopUnroller(initial, "out.c");
    REQUIRE(actual == expected);
}
