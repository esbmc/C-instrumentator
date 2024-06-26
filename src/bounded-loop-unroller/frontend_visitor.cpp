#include <bounded-loop-unroller/frontend_visitor.h>
#include <fmt/core.h>

#include <iostream>
#include <functional>
#include <algorithm>
#include <clang/Basic/SourceManager.h>
#include <sstream>

#define CONTINUE_SEARCH true
#define STOP_SEARCH false

namespace BoundedLoopUnroller {

const clang::IntegerLiteral *
get_init_value_from_decl(const clang::VarDecl *const decl) {
  if (!decl || !decl->hasInit())
    return nullptr;

  if (const clang::IntegerLiteral *const value_cast =
      llvm::dyn_cast<clang::IntegerLiteral>(decl->getInit()->IgnoreCasts())) 
    return value_cast;

  return nullptr;  
}

  std::string frontend_visitor::get_original_source(const clang::Stmt *const E) const {
    const char *e_start = sm.getCharacterData(E->getBeginLoc()),
      *e_end = sm.getCharacterData(E->getEndLoc());

    std::ostringstream oss;
    oss << std::string(e_start, e_end - e_start);

    if (const clang::UnaryOperator *const op =
        llvm::dyn_cast<clang::UnaryOperator>(E)) {
      oss << op->getOpcodeStr(op->getOpcode()).data();
    }

    return oss.str();
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

    const clang::IntegerLiteral * condition_limit;
      

    if (const clang::IntegerLiteral *const value_cast =
        llvm::dyn_cast<clang::IntegerLiteral>(bin_op->getRHS()->IgnoreCasts())) {
      condition_limit = value_cast;
    } else if (const clang::DeclRefExpr *const value_cast =
                   llvm::dyn_cast<clang::DeclRefExpr>(
                       bin_op->getRHS()->IgnoreCasts())) {
      if (!value_cast->getType().isConstant(context))
        return CONTINUE_SEARCH;
      const clang::VarDecl *const decl =
          llvm::dyn_cast<clang::VarDecl>(value_cast->getDecl());
      condition_limit = get_init_value_from_decl(decl);
    } else
      return CONTINUE_SEARCH;



    



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
            llvm::dyn_cast<clang::IntegerLiteral>(op_cast->getInit()->IgnoreCasts())) {
          cond_init_value = value_cast->getValue().getSExtValue();
          cond_end_value = condition_limit->getValue().getSExtValue();
        } else {
          op_cast->getInit()->IgnoreCasts()->dumpColor();
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

    RewriteForLoop(expr, number_of_unfolds, get_original_source(init), get_original_source(inc));  
    return CONTINUE_SEARCH;
  }

  void frontend_visitor::RewriteForLoop(clang::ForStmt *expr,
                                        int64_t unfold_number,
                                        const std::string_view &init,
                                        const std::string_view &increment) {
    
    const clang::Stmt *const body = expr->getBody();  
    std::vector<clang::Stmt *> my_loop;

    clang::FPOptionsOverride my_options;
    clang::CompoundStmt *stmt = clang::CompoundStmt::Create(context, my_loop, expr->getBody()->getBeginLoc(), expr->getBody()->getEndLoc());
    expr->setBody(stmt);


    // Just remove the loop in case of negative or 0;
    if (0 >= unfold_number || body->children().empty()) {
      rewriter.RemoveText(expr->getSourceRange());
      return;
    }

    std::ostringstream oss;
    oss << init << ";\n";

    std::string body_str = get_original_source(body);

    for (size_t counter = 0; counter < unfold_number; counter++) {
      if (!llvm::isa<clang::CompoundStmt>(body)) 
              oss << "{\n";
      oss << body_str;
      if (!llvm::isa<clang::CompoundStmt>(body)) 
              oss << ";";
      oss << "\n" << increment << ";\n";
      oss << "}\n";
    }

    rewriter.ReplaceText(expr->getSourceRange(), oss.str());
    
  }
}
