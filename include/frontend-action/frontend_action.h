#ifndef CINSTRUMENTATION_FRONTEND_ACTION_H
#define CINSTRUMENTATION_FRONTEND_ACTION_H
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Lex/Preprocessor.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include "frontend_consumer.h"

class frontend_action : public clang::ASTFrontendAction {
        public:

            virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
                    clang::CompilerInstance &CI, llvm::StringRef InFile) {
                llvm::errs() << "** Creating AST consumer for: " << InFile << "\n";
                rewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
                //return llvm::make_unique<MyASTConsumer>(TheRewriter,&CI.getASTContext());
                return std::make_unique<frontend_consumer>(rewriter);}

    void EndSourceFileAction() override {
        clang::SourceManager &SM = rewriter.getSourceMgr();
        llvm::errs() << "** EndSourceFileAction for: "
                     << SM.getFileEntryForID(SM.getMainFileID())->getName() << "\n";

        // Now emit the rewritten buffer.
        rewriter.getEditBuffer(SM.getMainFileID()).write(llvm::outs());
        std::error_code error_code;
        llvm::raw_fd_ostream outFile("output.txt", error_code, llvm::sys::fs::OF_None);
        rewriter.getEditBuffer(SM.getMainFileID()).write(outFile); // --> this will write the result to outFile
        outFile.close();
    }

protected:
    clang::Rewriter rewriter;
        };


#endif //CINSTRUMENTATION_FRONTEND_ACTION_H
