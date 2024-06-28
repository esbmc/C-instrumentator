#ifndef CINSTRUMENTATION_FRONTEND_ACTION_H
#define CINSTRUMENTATION_FRONTEND_ACTION_H

#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Lex/Preprocessor.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <clang/Tooling/Tooling.h>
#include "frontend_consumer.h"


/*
 * This is just the entry point for a Library/Plugin/etc.
 *
 * If you are looking for the instrumentation, go to frontend_visitor
 */
namespace BoundedLoopUnroller {
    class frontend_action : public clang::ASTFrontendAction {
    public:

        explicit frontend_action(const std::string file_output, std::string &rewriter_output) : file_output(
                file_output), rewriter_output(rewriter_output) {}

        virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer (
                clang::CompilerInstance &CI, llvm::StringRef InFile) {
          rewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());          
          return std::make_unique<frontend_consumer>(rewriter, CI.getSourceManager(), CI.getASTContext());
        }

        void EndSourceFileAction() override;


    protected:
        clang::Rewriter rewriter;
        const std::string file_output;
        std::string &rewriter_output;
    };
}

#endif //CINSTRUMENTATION_FRONTEND_ACTION_H
