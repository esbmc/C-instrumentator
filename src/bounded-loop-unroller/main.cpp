#include <__format/format_functions.h>
#include <bounded-loop-unroller/bounded-loop-unroller.h>
#include <cstdlib>
#include <fstream>
#include <iostream>

#include <algorithm>
#include <iterator>
#include <string>


template<typename InputIterator1, typename InputIterator2>
bool
range_equal(InputIterator1 first1, InputIterator1 last1,
        InputIterator2 first2, InputIterator2 last2)
{
    while(first1 != last1 && first2 != last2)
    {
        if(*first1 != *first2) return false;
        ++first1;
        ++first2;
    }
    return (first1 == last1) && (first2 == last2);
}

bool compare_files(const std::string_view& filename1, const std::string_view& filename2)
{
    std::ifstream file1(filename1);
    std::ifstream file2(filename2);

    std::istreambuf_iterator<char> begin1(file1);
    std::istreambuf_iterator<char> begin2(file2);

    std::istreambuf_iterator<char> end;

    return range_equal(begin1, end, begin2, end);
}

void create_copy(const std::string_view &filename1,
                 const std::string_view &filename2) {
    std::ifstream src(filename1, std::ios::binary);
    std::ofstream dst(filename2, std::ios::binary);

    dst << src.rdbuf();
}


int main(int argc, char **argv) {
  if (argc != 3)
        std::cerr << "./loop-unroller <input.c> <output.c>\n";

  const std::ifstream input(argv[1]);

  const char *tmp1 = "/tmp/_unrolled1.c";
  const char *tmp2 = "/tmp/_unrolled2.c";

  create_copy(argv[1], tmp1);
  
  // Lets normalize things!
  std::ofstream clang_format("/tmp/.clang-format");
  clang_format << "InsertBraces: True\n";
  clang_format.close();

  system(std::format("clang-format -style=file -i {}", tmp1).c_str());

  size_t counter = 0;
  while (1) {
        std::cout << "Running iteration " << counter++ << "\n";
            std::ifstream file1(tmp1);
        BoundedLoopUnroller::RunLoopUnroller(file1, tmp2);
        if (compare_files(tmp1, tmp2))
          break;

        create_copy(tmp2, tmp1);
  }

  create_copy(tmp2, argv[2]);

  system(std::format("clang-format -i {}", argv[2]).c_str());
  std::cout << "Done\n";
  return 0;
}
