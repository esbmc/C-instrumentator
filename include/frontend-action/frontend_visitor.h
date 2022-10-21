#ifndef CINSTRUMENTATION_FRONTEND_VISITOR_H
#define CINSTRUMENTATION_FRONTEND_VISITOR_H

#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/AST/DeclCXX.h>

class frontend_visitor
: public clang::RecursiveASTVisitor<frontend_visitor> {
public:
    bool VisitCXXRecordDecl(clang::CXXRecordDecl *Declaration) {
        // For debugging, dumping the AST nodes will show which nodes are already
        // being visited.
        Declaration->dump();

        // The return value indicates whether we want the visitation to proceed.
        // Return false to stop the traversal of the AST.
        return true;
    }
};

#endif //CINSTRUMENTATION_FRONTEND_VISITOR_H
