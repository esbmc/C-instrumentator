#include <bounded-loop-unroller/frontend_action.h>
#include <bounded-loop-unroller/frontend_consumer.h>
#include <bounded-loop-unroller/bounded-loop-unroller.h>

#include <iostream>
#include <format>
namespace {
  
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

}

namespace BoundedLoopUnroller {
  void frontend_action::EndSourceFileAction() {
    clang::SourceManager &SM = rewriter.getSourceMgr();

    // Now emit the rewritten buffer.
    llvm::raw_string_ostream ross(rewriter_output);
    rewriter.getEditBuffer(SM.getMainFileID()).write(ross);
    std::error_code error_code;
    llvm::raw_fd_ostream outFile(file_output, error_code, llvm::sys::fs::OF_None);
    rewriter.getEditBuffer(SM.getMainFileID()).write(outFile); // --> this will write the result to outFile
    outFile.close();
  }

  std::string RunLoopUnroller(const std::string input, const std::string output) {
    std::string result;
    clang::tooling::runToolOnCode(std::make_unique<frontend_action>(output, result), input);
    return result;
  }
  std::string RunLoopUnroller(const std::ifstream &input, const std::string output) {
    std::ostringstream oss;
    oss << input.rdbuf();
    return RunLoopUnroller(oss.str(), output);
  }

  void Run(const std::string_view &arg1, const std::string_view &arg2) {
      
    const std::ifstream input(arg1);

    const char *tmp1 = "/tmp/_unrolled1.c";
    const char *tmp2 = "/tmp/_unrolled2.c";

    create_copy(arg1, tmp1);
  
    // Lets normalize things!
    std::ofstream clang_format("/tmp/.clang-format");
    clang_format << "InsertBraces: True\nBasedOnStyle: LLVM\n";
    clang_format.close();

    system(std::format("clang-format -style=file -i {}", tmp1).c_str());

    size_t counter = 0;
    while (1) {
      std::cout << "Running iteration " << counter++ << "\n";
      std::ifstream file1(tmp1);
      RunLoopUnroller(file1, tmp2);
      if (compare_files(tmp1, tmp2))
        break;

      create_copy(tmp2, tmp1);
    }

    system(std::format("clang-format -i {}", tmp2).c_str());
    create_copy(tmp2, arg2);

    
    
    std::cout << "Done\n";      
  }
}
