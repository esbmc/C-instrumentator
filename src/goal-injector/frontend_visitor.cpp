#include <goal-injector/frontend_visitor.h>

namespace GoalInjector {
    bool frontend_visitor::VisitWhileStmt(clang::WhileStmt *expr) {
        //rewriter.InsertTextBefore(expr->getBeginLoc(), "ASD();");
        InjectOnCompoundStmt(static_cast<clang::CompoundStmt *>(expr->getBody()));
        // The return value indicates whether we want the visitation to proceed.
        // Return false to stop the traversal of the AST.
        return true;
    }

    void frontend_visitor::InjectOnStmt(clang::Stmt *stmt) {
        if (static_cast<clang::CompoundStmt *>(stmt)) {
            InjectOnCompoundStmt(static_cast<clang::CompoundStmt *>(stmt));
        }
        stmt->dump();
        assert(!"Stmt not supported");
    }

    void frontend_visitor::InjectOnCompoundStmt(clang::CompoundStmt *stmt) {
        // TODO: Not sure whether begin or end location is the best
        rewriter.InsertTextAfter(stmt->getEndLoc(), goal_str);
    }
}