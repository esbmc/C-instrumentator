#ifndef CINSTRUMENTATION_FRONTEND_VISITOR_H
#define CINSTRUMENTATION_FRONTEND_VISITOR_H

#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/AST/DeclCXX.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <sstream>

/**
 * This is where the instrumentatoin takes place.
 *
 * 1. Clang will automatically call for every VisitXXX(XXX *). For this instrumentation, we are mainly interested
 *    in /IfStmt/, /WhileStmt/ and all other statements that cause branching.
 *    Check the full statements list here: https://clang.llvm.org/doxygen/classclang_1_1Stmt.html
 *
 * 2. After we find one of these /branching/ statements, we then instrument its body
 *
 * 3. We can instrument by:
 *   a. Using the same approach of FuSeBMC (i.e., GOAL_1:, GOAL_2:, etc.)
 *   b. Add an assert(0), reach_error (or equivalent).
 *
 * For now, I will go with the option 3b, as it seems easier to integrate with ESBMC. However, it can cause problems
 * during source code rewriting. Specifically, because C allows one-instruction if/while/etc commands.
 * This means that if the body of the interesting expressions are not /CompoundStmt/ we will need to manually create a
 * scope.
 */
namespace GoalInjector {
    class frontend_visitor
            : public clang::RecursiveASTVisitor<frontend_visitor> {
    public:
        explicit frontend_visitor(clang::Rewriter &rewriter) : rewriter(rewriter) {}

        bool VisitWhileStmt(clang::WhileStmt *expr);

        /*
         * TODO:
         *
         * IF
         * SwitchCase
         * FOR
         * RETURN
         */

    protected:
        void InjectOnStmt(clang::Stmt *stmt);

        void InjectOnCompoundStmt(clang::CompoundStmt *stmt);

        void InjectOnNonCompoundStmt(clang::Stmt *stmt);

    private:
        clang::Rewriter &rewriter;
        const std::string goal_str = "reach_error();";
        const std::string goal_block_str = "{reach_error();";

    };
}
#endif //CINSTRUMENTATION_FRONTEND_VISITOR_H
