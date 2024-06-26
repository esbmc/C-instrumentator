#pragma once

#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/AST/DeclCXX.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <sstream>
#include <string_view>

/**
 * This is where the instrumentatoin takes place.
 *
 * 1. Clang will automatically call for every VisitXXX(XXX *). For this instrumentation, we are mainly interested
 *    in /ForStmt/, as these are the loops that we are interested
 *    Check the full statements list here: https://clang.llvm.org/doxygen/classclang_1_1Stmt.html
 *
 * 2. After we find one of these /for/ statements that is bounded, we replace the body
 *
 * 3. We instrument by adding comments with locations to be replaced. 
 *
 * NOTE: There is another way to get the original source code and the keep replacing. I am having trouble with that for some reason
 */
namespace BoundedLoopUnroller {
    class frontend_visitor
            : public clang::RecursiveASTVisitor<frontend_visitor> {
    public:
      explicit frontend_visitor(clang::Rewriter &rewriter, clang::SourceManager &context) : rewriter(rewriter), context(context) {}

        bool VisitForStmt(clang::ForStmt *expr);

      // TODO: other loops, e.g.: while, do-while, backwards goto

    protected:
      void RewriteForLoop(clang::ForStmt *expr, int64_t unfold_number,
                          const std::string_view &init,
                          const std::string_view &increment);
      std::string get_original_source(const clang::Stmt *const E) const;

    private:
       	int counter = 0;
        clang::Rewriter &rewriter;
        clang::SourceManager &context;      
    };
}
