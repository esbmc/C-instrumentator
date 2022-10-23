#ifndef CINSTRUMENTATION_FRONTEND_CONSUMER_H
#define CINSTRUMENTATION_FRONTEND_CONSUMER_H


#include <clang/AST/ASTConsumer.h>
#include "frontend_visitor.h"

/*
 * This is just a top-level of how a translation unit should be traversed.
 *
 * If you are looking for the instrumentation, go to frontend_visitor
 */


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


#endif //CINSTRUMENTATION_FRONTEND_CONSUMER_H
