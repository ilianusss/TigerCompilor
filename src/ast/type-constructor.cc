/**
 ** \file ast/type-constructor.cc
 ** \brief Implementation of ast::TypeConstructor.
 */

#include <ast/type-constructor.hh>
#include <ast/visitor.hh>

namespace ast
{
  // FIXED
  TypeConstructor::TypeConstructor()
    : type_constructor_(nullptr)
  {}

  TypeConstructor::~TypeConstructor() {}
} // namespace ast
