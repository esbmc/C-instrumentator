#ifndef CINSTRUMENTATION_FRONTEND_VISITOR_H
#define CINSTRUMENTATION_FRONTEND_VISITOR_H

#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/AST/DeclCXX.h>
#include <clang/Rewrite/Core/Rewriter.h>

class frontend_visitor
: public clang::RecursiveASTVisitor<frontend_visitor> {
public:
    explicit frontend_visitor(clang::Rewriter &rewriter) : rewriter(rewriter) {}

    bool VisitCXXRecordDecl(clang::CXXRecordDecl *Declaration) {
        // For debugging, dumping the AST nodes will show which nodes are already
        // being visited.
        Declaration->dump();
        rewriter.ReplaceText(Declaration->getBeginLoc(), "ASD();");

        // The return value indicates whether we want the visitation to proceed.
        // Return false to stop the traversal of the AST.
        return true;
    }
private:
    clang::Rewriter &rewriter;
};

#endif //CINSTRUMENTATION_FRONTEND_VISITOR_H
