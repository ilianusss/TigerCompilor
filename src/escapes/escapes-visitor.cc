/**
 ** \file escapes/escapes-visitor.cc
 ** \brief Implementation for escapes/escapes-visitor.hh.
 */

#include <ast/all.hh>
#include <escapes/escapes-visitor.hh>
#include <misc/contract.hh>

namespace escapes
{
  // FIXED: EscapesVisitor implementation added
  EscapesVisitor::EscapesVisitor()
    : scoped_map_()
    , in_primitive(false)
  {}

  void EscapesVisitor::operator()(ast::SimpleVar& e)
  {
    misc::symbol identifier = e.name_get();
    auto current_scope = scoped_map_.get_last_scope();
    if (current_scope.find(identifier) != current_scope.end())
      {
        e.def_get()->is_escapable_set(false);
      }
    super_type::operator()(e);
  }

  void EscapesVisitor::operator()(ast::VarDec& e)
  {
    misc::symbol identifier = e.name_get();
    if (in_primitive)
      e.is_escapable_set(false);
    else
      e.is_escapable_set(true);
    bool _ = false;
    scoped_map_.put(identifier, _);
    super_type::operator()(e);
  }

  void EscapesVisitor::operator()(ast::FunctionDec& e)
  {
    scoped_map_.scope_begin();
    if (e.body_get() == nullptr)
      in_primitive = true;
    super_type::operator()(e);
    in_primitive = false;
    scoped_map_.scope_end();
  }

} // namespace escapes
