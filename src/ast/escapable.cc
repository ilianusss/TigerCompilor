/**
 ** \file ast/escapable.cc
 ** \brief Implementation of ast::Escapable.
 */

#include <ast/escapable.hh>
#include <ast/visitor.hh>

namespace ast
{
  //FIXED: Complete implementation added

  Escapable::Escapable()
    : is_escapable_(true)
  {}

  bool Escapable::is_escapable_get() const { return is_escapable_; }

  void Escapable::is_escapable_set(bool is_escapable)
  {
    is_escapable_ = is_escapable;
  }

} // namespace ast
