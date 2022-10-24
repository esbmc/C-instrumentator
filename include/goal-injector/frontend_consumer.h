#ifndef CINSTRUMENTATION_FRONTEND_CONSUMER_H
#define CINSTRUMENTATION_FRONTEND_CONSUMER_H

#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Lex/Preprocessor.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <clang/Tooling/Tooling.h>
#include <clang/AST/ASTConsumer.h>
#include "frontend_visitor.h"

/*
 * This is just a top-level of how a translation unit should be traversed.
 *
 * If you are looking for the instrumentation, go to frontend_visitor
 */
namespace GoalInjector {

    class frontend_consumer : public clang::ASTConsumer {
    public:
        explicit frontend_consumer(clang::Rewriter &rewriter) : rewriter(rewriter), Visitor(rewriter) {}

        virtual void HandleTranslationUnit(clang::ASTContext &Context) {
            Visitor.TraverseAST(Context);
        }

    private:
        clang::Rewriter &rewriter;
        // A RecursiveASTVisitor implementation.
        frontend_visitor Visitor;
    };
}

#endif //CINSTRUMENTATION_FRONTEND_CONSUMER_H
