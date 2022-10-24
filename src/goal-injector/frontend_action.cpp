#include <goal-injector/frontend_action.h>
#include <goal-injector/frontend_consumer.h>
#include <goal-injector/goal-injector.h>

namespace GoalInjector {
    void frontend_action::EndSourceFileAction() {
        clang::SourceManager &SM = rewriter.getSourceMgr();

// Now emit the rewritten buffer.
        llvm::raw_string_ostream ross(rewriter_output);
        rewriter.getEditBuffer(SM.getMainFileID()).write(ross);
        std::error_code error_code;
        llvm::raw_fd_ostream outFile("output.txt", error_code, llvm::sys::fs::OF_None);
        rewriter.getEditBuffer(SM.getMainFileID()).write(outFile); // --> this will write the result to outFile
        outFile.close();
    }

    std::string RunGoalInjector(const std::string input, const std::string output) {
        std::string result;
        clang::tooling::runToolOnCode(std::make_unique<frontend_action>(output, result), input);
        return result;
    }
}