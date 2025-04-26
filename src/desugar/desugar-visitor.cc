/**
 ** \file desugar/desugar-visitor.cc
 ** \brief Implementation of desugar::DesugarVisitor.
 */

#include <ast/all.hh>
#include <ast/libast.hh>
#include <desugar/desugar-visitor.hh>
#include <misc/algorithm.hh>
#include <misc/symbol.hh>
#include <parse/libparse.hh>
#include <parse/tweast.hh>
#include "ast/op-exp.hh"
#include "type/builtin-types.hh"
#include "type/libtype.hh"
#include "type/type-checker.hh"
#include "type/type.hh"

namespace desugar
{
  DesugarVisitor::DesugarVisitor(bool desugar_for_p, bool desugar_string_cmp_p)
    : super_type()
    , desugar_for_p_(desugar_for_p)
    , desugar_string_cmp_p_(desugar_string_cmp_p)
  {}

  /*-----------------------------.
  | Desugar string comparisons.  |
  `-----------------------------*/
  void DesugarVisitor::operator()(const ast::OpExp& e)
  {
    // FIXED: Some code was deleted here.
    ast::OpExp::Oper op_type = e.oper_get();
    if (e.left_get().get_type() == e.right_get().get_type()
        && *e.left_get().get_type() == type::String::instance())
      {
        parse::Tweast in;

        if (op_type == ast::OpExp::Oper::eq) // '=' operator
          in << "streq(" << recurse(e.left_get()) << ","
             << recurse(e.right_get()) << ")";
        else if (op_type == ast::OpExp::Oper::ne) // '<>' operator
          in << "streq(" << recurse(e.left_get()) << ","
             << recurse(e.right_get()) << ") <> 0";
        else if (op_type == ast::OpExp::Oper::lt) // '<' operator
          in << "strcmp(" << recurse(e.left_get()) << ","
             << recurse(e.right_get()) << ") < 0";
        else if (op_type == ast::OpExp::Oper::le) // '<=' operator
          in << "strcmp(" << recurse(e.left_get()) << ","
             << recurse(e.right_get()) << ") <= 0";
        else if (op_type == ast::OpExp::Oper::gt) // '>' operator
          in << "strcmp(" << recurse(e.left_get()) << ","
             << recurse(e.right_get()) << ") > 0";
        else if (op_type == ast::OpExp::Oper::ge) // '>=' operator
          in << "strcmp(" << recurse(e.left_get()) << ","
             << recurse(e.right_get()) << ") >= 0";
        result_ = std::get<ast::Exp*>(parse::parse(in));
      }
    else
      super_type::operator()(e);
  }

  /*----------------------.
  | Desugar `for' loops.  |
  `----------------------*/

  /*<<-
    Desugar `for' loops as `while' loops:

       for i := lo to hi do
         body

     is transformed as:

       let
         var _lo := lo
         var _hi := hi
         var i := _lo
       in
         if i <= _hi then
           while 1 do
             (
               body;
               if i = _hi then
                 break;
               i := i + 1
             )
       end

     Notice that:

     - a `_hi' variable is introduced so that `hi' is evaluated only
       once;

     - a `_lo' variable is introduced to prevent `i' from being in the
       scope of `_hi';

     - a first test is performed before entering the loop, so that the
       loop condition becomes `i < _hi' (instead of `i <= _hi'); this
       is done to prevent overflows on INT_MAX.
       ->>*/

  void DesugarVisitor::operator()(const ast::ForExp& e)
  {
    // FIXED: Some code was deleted here.
    parse::Tweast in;

    misc::symbol var = e.vardec_get().name_get();

    in << "let\n"
       << "var _lo := " << *recurse(e.vardec_get().init_get()) << "\n"
       << "var _hi := " << *recurse(e.hi_get()) << "\n"
       << "var " << var << " := _lo\n"
       << "in\n"
       << "if " << var << " <= _hi then\n"
       << "while 1 do\n"
       << "(\n"
       << *recurse(e.body_get()) << ";\n"
       << "if " << var << " = _hi then\n"
       << "break;\n"
       << "" << var << " := " << var << " + 1\n"
       << ")\n"
       << "end";

    result_ = std::get<ast::Exp*>(parse::parse(in));
  }

} // namespace desugar
