#include <goal-injector/frontend_visitor.h>
#include <memory>

namespace GoalInjector {
    bool frontend_visitor::VisitWhileStmt(clang::WhileStmt *expr) {
        InjectOnStmt(expr->getBody());
        return true;
    }

    void frontend_visitor::InjectOnStmt(clang::Stmt *stmt) {

        if (stmt->getStmtClass() == clang::Stmt::CompoundStmtClass) {
            InjectOnCompoundStmt(static_cast<clang::CompoundStmt *>(stmt));
        } else {
            InjectOnNonCompoundStmt(stmt);
        }
    }

    void frontend_visitor::InjectOnCompoundStmt(clang::CompoundStmt *stmt) {
        // TODO: Not sure whether begin or end location is the best
        rewriter.InsertTextAfter(stmt->getBeginLoc().getLocWithOffset(1), goal_str);
    }

    void frontend_visitor::InjectOnNonCompoundStmt(clang::Stmt *stmt) {
        // TODO: Not sure whether begin or end location is the best
        rewriter.InsertTextAfter(stmt->getBeginLoc(), goal_block_str);
        rewriter.InsertTextAfter(stmt->getEndLoc().getLocWithOffset(1), "}");
    }
}