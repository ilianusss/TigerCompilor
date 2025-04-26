/**
 ** \file ast/typable.cc
 ** \brief Implementation of ast::Typable.
 */

#include <ast/typable.hh>
#include <ast/visitor.hh>

namespace ast
{
  // FIXED
  Typable::Typable()
    : type_(nullptr)
  {}

  Typable::~Typable() {}
} // namespace ast
