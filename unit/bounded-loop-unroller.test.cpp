#include <catch2/catch_test_macros.hpp>
#include <bounded-loop-unroller/bounded-loop-unroller.h>

#include <filesystem>
std::string run_test(std::string input) {
  std::ofstream output("input.c");
  output << input;
  output.close();

  BoundedLoopUnroller::Run("input.c", "out.c");

  std::ifstream read("out.c");
  std::ostringstream oss;
  oss << read.rdbuf();
  return oss.str();
}



TEST_CASE("One-line empty For-Loops are removed", "[for]") {
    auto initial = R"(
int main() { for (int i = 0; i < 3; i++) ; return 0; })";
    auto expected = R"(
int main() { return 0; })";
    auto actual = run_test(initial);
    REQUIRE(actual == expected);
}



TEST_CASE("Multi-line empty For-Loops are removed", "[for]") {
    auto initial = R"(
int main() { for (int i = 0; i < 3; i++) {

}
return 0; })";
    auto expected = R"(
int main() { return 0; })";
    auto actual = run_test(initial);
    REQUIRE(actual == expected);
}



TEST_CASE("One-line < For-Loops are detected", "[for]") {
    auto initial = R"(
int main() { int a; for (int i = 0; i < 3; i++) a++; return 0; })";
    auto expected = R"(
int main() {
  int a;
  int i = 0;
  {
    a++;

    i++;
  }
  {
    a++;

    i++;
  }
  {
    a++;

    i++;
  }

  return 0;
})";
    auto actual = run_test(initial);
    REQUIRE(actual == expected);
}


TEST_CASE("One-line unsigned For-Loops are detected", "[for]") {
    auto initial = R"(
int main() { int a; for (unsigned int i = 0; i < 3; i++) a++; return 0; })";
    auto expected = R"(
int main() {
  int a;
  unsigned int i = 0;
  {
    a++;

    i++;
  }
  {
    a++;

    i++;
  }
  {
    a++;

    i++;
  }

  return 0;
})";
    auto actual = run_test(initial);
    REQUIRE(actual == expected);
}



TEST_CASE("One-line <= For-Loops are detected", "[for]") {
    auto initial = R"(
int main() { int a; for (int i = 0; i <= 3; i++) a++; return 0; })";
    auto expected = R"(
int main() {
  int a;
  int i = 0;
  {
    a++;

    i++;
  }
  {
    a++;

    i++;
  }
  {
    a++;

    i++;
  }
  {
    a++;

    i++;
  }

  return 0;
})";
    auto actual = run_test(initial);
    REQUIRE(actual == expected);
}



TEST_CASE("One-line > For-Loops are detected", "[for]") {
    auto initial = R"(
int main() { int a; for (int i = 3; i > 1; i--) a++; return 0; })";
    auto expected = R"(
int main() {
  int a;
  int i = 3;
  {
    a++;

    i--;
  }
  {
    a++;

    i--;
  }

  return 0;
})";
    auto actual = run_test(initial);
    REQUIRE(actual == expected);
}


TEST_CASE("One-line >= For-Loops are detected", "[for]") {
    auto initial = R"(
int main() { int a; for (int i = 4; i >= 1; i--) a++; return 0; })";
    auto expected = R"(
int main() {
  int a;
  int i = 4;
  {
    a++;

    i--;
  }
  {
    a++;

    i--;
  }
  {
    a++;

    i--;
  }
  {
    a++;

    i--;
  }

  return 0;
})";
    auto actual = run_test(initial);
    REQUIRE(actual == expected);
}



TEST_CASE("Multi-line For-Loops are detected", "[for]") {
    auto initial = R"(
int main() {
int a;
for (int i = 1; i < 3; i++) {
a++;
}
return 0; })";
    auto expected = R"(
int main() {
  int a;
  int i = 1;
  {
    a++;

    i++;
  }
  {
    a++;

    i++;
  }

  return 0;
})";
    auto actual = run_test(initial);
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
  int i = 1;
  {
    a++;

    i++;
  }
  {
    a++;

    i++;
  }
  {
    a++;

    i++;
  }
  {
    a++;

    i++;
  }

  return 0;
})";
    auto actual = run_test(initial);
    REQUIRE(actual == expected);
}



TEST_CASE("Multi-line Commented For-Loops are detected", "[for]") {
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
  int i = 1;
  {
    a++;

    i++;
  }
  {
    a++;

    i++;
  }
  {
    a++;

    i++;
  }
  {
    a++;

    i++;
  }

  return 0;
})";
    auto actual = run_test(initial);
    REQUIRE(actual == expected);
}



TEST_CASE("Nested For-Loops are detected", "[for]") {
    auto initial = R"(
int main() { int a; for (int i = 0; i < 3; i++) for(int j = 0; j < 2; j++) a++; return 0; })";
    auto expected = R"(
int main() {
  int a;
  int i = 0;
  {
    int j = 0;
    {
      a++;

      j++;
    }
    {
      a++;

      j++;
    }

    i++;
  }
  {
    int j = 0;
    {
      a++;

      j++;
    }
    {
      a++;

      j++;
    }

    i++;
  }
  {
    int j = 0;
    {
      a++;

      j++;
    }
    {
      a++;

      j++;
    }

    i++;
  }

  return 0;
})";
    auto actual = run_test(initial);
    REQUIRE(actual == expected);
}

