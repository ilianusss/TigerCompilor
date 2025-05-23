/**
 ** \file type/type-checker.hxx
 ** \brief Inline methods of type::TypeChecker.
 */

#pragma once

#include <ast/all.hh>
#include <type/pretty-printer.hh>
#include <type/type-checker.hh>
#include <type/types.hh>

namespace type
{
  namespace
  {
    const Nil nil_error_instance{};

  }

  /*----------------.
  | Setting types.  |
  `----------------*/

  template <typename NodeType>
  void TypeChecker::type_default(NodeType& e, const type::Type* type)
  {
    // FIXED
    if (!e.get_type())
      e.set_type(type);
  }

  template <typename NodeType>
  void TypeChecker::created_type_default(NodeType& e, const type::Type* type)
  {
    // FIXED
    if (!e.get_type_constructor())
      e.set_type_constructor(type);
  }

  template <typename NodeType>
  void TypeChecker::type_set(NodeType& e, const type::Type* type)
  {
    // FIXED
    e.set_type(type);
  }

  /*-----------------.
  | Error handling.  |
  `-----------------*/

  template <typename T>
  void
  TypeChecker::error(const ast::Ast& ast, const std::string& msg, const T& exp)
  {
    error_ << misc::error::error_type::type << ast.location_get() << ": " << msg
           << ": " << exp << std::endl;
  }

  template <typename T, typename U>
  void
  TypeChecker::error_and_recover(T& loc, const std::string& msg, const U& exp)
  {
    error(loc, msg, exp);
    loc.type_set(&nil_error_instance);
  }

  template <typename NodeType>
  void TypeChecker::check_type(NodeType& e,
                               const std::string& s,
                               const Type& expected)
  {
    // FIXED
    if (e.get_type() && !e.get_type()->compatible_with(expected))
      {
        error(e, s);
        e.set_type(&expected);
      }
  }

  /*------------.
  | Functions.  |
  `------------*/

  template <typename Routine_Type, typename Routine_Node>
  void TypeChecker::visit_routine_body(Routine_Node& e)
  {
    // FIXED
    if (!e.body_get())
      return;
    type(*e.body_get());
    check_types(e, "result type",
                dynamic_cast<const Routine_Type*>(e.get_type())->result_get(),
                "body type", *e.body_get()->get_type());
  }

} // namespace type
