#include <bounded-loop-unroller/frontend_visitor.h>
#include <fmt/core.h>

#include <iostream>
#include <functional>
#include <algorithm>
#include <clang/Basic/SourceManager.h>

#define CONTINUE_SEARCH true
#define STOP_SEARCH false

namespace BoundedLoopUnroller {

 std::string frontend_visitor::get_original_source(const clang::Stmt *const E) const {
   const char *e_start = context.getCharacterData(E->getBeginLoc()),
              *e_end = context.getCharacterData(E->getEndLoc());

   return std::string(e_start, e_end - e_start);
}

  bool check_assignment_over_var(const clang::Stmt *const E,
                                 const clang::ValueDecl *const V) {

    if (!E)
      return false;
    if (const clang::UnaryOperator *const op =
        llvm::dyn_cast<clang::UnaryOperator>(E)) {
      const clang::DeclRefExpr *const reference =
        llvm::dyn_cast<clang::DeclRefExpr>(op->getSubExpr()->IgnoreCasts());

      // TODO: This is too aggresive! :)
      if (reference && reference->getDecl() == V)
        return true;
    } else if (const clang::BinaryOperator *const op =
               llvm::dyn_cast<clang::BinaryOperator>(E)) {
      if (op->isAssignmentOp()) {
        const clang::DeclRefExpr *const reference =
          llvm::dyn_cast<clang::DeclRefExpr>(op->getLHS()->IgnoreCasts());

        if (reference && reference->getDecl() == V)
          return true;
      }
    }

    for (const auto &e : E->children())
      if (check_assignment_over_var(e, V))
        return true;

    return false;
  }

  bool frontend_visitor::VisitForStmt(clang::ForStmt *expr) {

    ///////////////////////////////////////////////////////////////
    // 1. Identify all elements: `for(INIT, COND, INC) { BODY }` //
    ///////////////////////////////////////////////////////////////
    const clang::DeclStmt *const init = llvm::dyn_cast<clang::DeclStmt>(expr->getInit());
    const clang::Expr *const inc = expr->getInc();
    const clang::Expr *const cond = expr->getCond();
    const clang::Stmt *const body = expr->getBody();

    if (!init || !inc || !cond || !body)
      return CONTINUE_SEARCH;

    int64_t cond_init_value;
    int64_t cond_end_value;


    ////////////////////////////////
    // 2. Check for preconditions //
    ////////////////////////////////

    // a. loop must be monothonic
    // b. the condition must be over one variable

    // TODO: Right now we only support `var` op `number`, where op is <, <=, >,
    // >=.
    if (!cond || !llvm::isa<clang::BinaryOperator>(cond))
      return false;


    const clang::BinaryOperator *const bin_op =
      llvm::dyn_cast<clang::BinaryOperator>(cond);
    assert(bin_op);

    const clang::DeclRefExpr *const condition_var =
      llvm::dyn_cast<clang::DeclRefExpr>(bin_op->getLHS()->IgnoreCasts());
    const clang::IntegerLiteral *const condition_limit =
        llvm::dyn_cast<clang::IntegerLiteral>(bin_op->getRHS()->IgnoreCasts());


    const std::array SUPPORTED_OP{clang::BinaryOperator::Opcode::BO_GE, clang::BinaryOperator::Opcode::BO_GT, clang::BinaryOperator::Opcode::BO_LE, clang::BinaryOperator::Opcode::BO_LT, };

    if (!condition_var || !condition_limit || (std::find(SUPPORTED_OP.begin(), SUPPORTED_OP.end(), bin_op->getOpcode()) == SUPPORTED_OP.end()))
      return CONTINUE_SEARCH;

    // Check if COND var is the same as initialized
    bool init_var_check = false;
    for (auto op : init->decls()) {
      const clang::VarDecl *const op_cast = llvm::dyn_cast<clang::VarDecl>(op);
      if (op_cast && condition_var->getDecl() == op_cast && op_cast->hasInit()) {
        init_var_check = true;
        if (const clang::IntegerLiteral *const value_cast =
            llvm::dyn_cast<clang::IntegerLiteral>(op_cast->getInit())) {
          cond_init_value = value_cast->getValue().getSExtValue();
          cond_end_value = condition_limit->getValue().getSExtValue();
        }
      }
    }    

    if (!init_var_check)
      return CONTINUE_SEARCH;

    
    // Check if INC is the same as intialized
    bool inc_var_check = false;
    bool inc_is_increasing = false;
    if (const clang::UnaryOperator *const op =
        llvm::dyn_cast<clang::UnaryOperator>(inc)) {
      switch (op->getOpcode()) {
      case clang::UnaryOperator::Opcode::UO_PostInc:
      case clang::UnaryOperator::Opcode::UO_PreInc:
        inc_is_increasing = true;
        break;
      case clang::UnaryOperator::Opcode::UO_PostDec:
      case clang::UnaryOperator::Opcode::UO_PreDec:
        inc_is_increasing = false;
        break;
      default:
        return CONTINUE_SEARCH;
      }

      const clang::DeclRefExpr *const reference =
        llvm::dyn_cast<clang::DeclRefExpr>(op->getSubExpr()->IgnoreCasts());


      if (reference && reference->getDecl() == condition_var->getDecl())
        inc_var_check = true;

    }

    if (!inc_var_check)
      return CONTINUE_SEARCH;

    // Check whether the BODY contains an assignment over the condition_variable
    if (check_assignment_over_var(body, condition_var->getDecl()))
      return CONTINUE_SEARCH;


    /////////////////////////////////////
    // 3. Compute number of iterations //
    /////////////////////////////////////

    int64_t number_of_unfolds;

    if (!inc_is_increasing &&
        (bin_op->getOpcode() == clang::BinaryOperator::Opcode::BO_GE ||
         bin_op->getOpcode() == clang::BinaryOperator::Opcode::BO_GT)) {
      if (bin_op->getOpcode() == clang::BinaryOperator::Opcode::BO_GE)
        cond_end_value--;

      number_of_unfolds = cond_init_value - cond_end_value;      
    }
    else if (inc_is_increasing && (bin_op->getOpcode() == clang::BinaryOperator::Opcode::BO_LE || bin_op->getOpcode() == clang::BinaryOperator::Opcode::BO_LT )) {
      if (bin_op->getOpcode() == clang::BinaryOperator::Opcode::BO_LE)
        cond_end_value++;

      number_of_unfolds = cond_end_value - cond_init_value;
    }
    else
      return CONTINUE_SEARCH;


    auto increment = fmt::format("{}{}", get_original_source(inc), inc_is_increasing ? "++" : "--"); 

    RewriteForLoop(expr, number_of_unfolds, get_original_source(init), increment);  
    return CONTINUE_SEARCH;
  }


  
  void frontend_visitor::RewriteForLoop(clang::ForStmt *expr, int64_t unfold_number, const std::string_view &init, const std::string_view &increment) {
    const clang::Stmt *const body = expr->getBody();


    // Just remove the loop in case of negative or 0;
    if (0 >= unfold_number || body->children().empty()) {
      rewriter.RemoveText(expr->getSourceRange());
      return;
    }
    

    rewriter.InsertTextBefore(
        expr->getBeginLoc(),
        fmt::format(
            "\n// LOOP {} START. ITERATIONS: {}. INIT: {};. INCR: {};.\n",
            counter, unfold_number, init, increment));

    
    rewriter.InsertTextAfter(
        body->child_begin()->getBeginLoc(),
        fmt::format("\n// LOOP {} BODY START\n", counter));


    if (llvm::isa<clang::CompoundStmt>(body)) {
      rewriter.InsertTextBefore(
          expr->getEndLoc(), fmt::format("\n// LOOP {} BODY END\n", counter));
      rewriter.InsertTextAfterToken(expr->getEndLoc(),
                                    fmt::format("\n// LOOP {} END\n", counter));
    } else {
      rewriter.InsertTextAfterToken(expr->getEndLoc().getLocWithOffset(2),
                                    fmt::format("\n// LOOP {} BODY END\n// LOOP {} END", counter, counter));
    }
    
    counter++;
  }
}
