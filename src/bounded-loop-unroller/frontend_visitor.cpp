#include <bounded-loop-unroller/frontend_visitor.h>
#include <fmt/core.h>

namespace BoundedLoopUnroller {
    bool frontend_visitor::VisitWhileStmt(clang::WhileStmt *expr) {
        InjectOnStmt(expr->getBody());
        return true;
    }

    bool frontend_visitor::VisitIfStmt(clang::IfStmt *expr) {
        InjectOnStmt(expr->getThen());
        InjectOnStmt(expr->getElse());
        return true;
    }

    bool frontend_visitor::VisitForStmt(clang::ForStmt *expr) {
        InjectOnStmt(expr->getBody());
        return true;
    }

    bool frontend_visitor::VisitFunctionDecl(clang::FunctionDecl *F) {
        InjectOnStmt(F->getBody());
        return true;
    }


    void frontend_visitor::InjectOnStmt(clang::Stmt *stmt) {
        if (!stmt)
            return;

        if (stmt->getStmtClass() == clang::Stmt::CompoundStmtClass) {
            InjectOnCompoundStmt(static_cast<clang::CompoundStmt *>(stmt));
        } else {
            InjectOnNonCompoundStmt(stmt);
        }
    }

    std::string frontend_visitor::goal_str() {
	    return fmt::format("__ESBMC_assert(0, \"{}\");", counter++);
    }
    
    std::string frontend_visitor::goal_block_str() {
	    return fmt::format("{}__ESBMC_assert(0, \"{}\");", "{", counter++);
    }
    void frontend_visitor::InjectOnCompoundStmt(clang::CompoundStmt *stmt) {
        // TODO: Not sure whether begin or end location is the best
        rewriter.InsertTextAfter(stmt->getBeginLoc().getLocWithOffset(1), goal_str());
    }

    void frontend_visitor::InjectOnNonCompoundStmt(clang::Stmt *stmt) {
        // TODO: Not sure whether begin or end location is the best
        rewriter.InsertTextAfter(stmt->getBeginLoc(), goal_block_str());
        rewriter.InsertTextAfter(stmt->getEndLoc().getLocWithOffset(1), "}");
    }
}
