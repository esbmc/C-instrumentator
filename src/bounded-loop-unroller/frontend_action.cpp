#include <bounded-loop-unroller/frontend_action.h>
#include <bounded-loop-unroller/frontend_consumer.h>
#include <bounded-loop-unroller/bounded-loop-unroller.h>

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
}
