#ifndef CINSTRUMENTATION_FRONTEND_CONSUMER_H
#define CINSTRUMENTATION_FRONTEND_CONSUMER_H


#include <clang/AST/ASTConsumer.h>
#include "frontend_visitor.h"

class frontend_consumer : public clang::ASTConsumer {
public:
    explicit frontend_consumer(clang::Rewriter &rewriter) : rewriter(rewriter), Visitor(rewriter) {}

    virtual void HandleTranslationUnit(clang::ASTContext &Context) {
        // Traversing the translation unit decl via a RecursiveASTVisitor
        // will visit all nodes in the AST.
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }
private:
    // A RecursiveASTVisitor implementation.
    clang::Rewriter &rewriter;
    frontend_visitor Visitor;
};


#endif //CINSTRUMENTATION_FRONTEND_CONSUMER_H
