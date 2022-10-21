#ifndef CINSTRUMENTATION_FRONTEND_ACTION_H
#define CINSTRUMENTATION_FRONTEND_ACTION_H
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Lex/Preprocessor.h>
#include "frontend_consumer.h"

class frontend_action : public clang::ASTFrontendAction {
        public:
            virtual std::unique_ptr<clang::ASTConsumer> frontend_action::CreateASTConsumer(
                    clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
                return std::make_unique<frontend_consumer>();}
        };


#endif //CINSTRUMENTATION_FRONTEND_ACTION_H
