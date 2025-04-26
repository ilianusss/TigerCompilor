/**
 ** \file ast/type-constructor.hxx
 ** \brief Inline methods of ast::TypeConstructor.
 */

#pragma once

#include <ast/type-constructor.hh>
#include <type/types.hh>

namespace ast
{
  // FIXED
  inline const type::Type* TypeConstructor::get_type_constructor() const
  {
    return type_constructor_;
  }

  inline void TypeConstructor::set_type_constructor(const type::Type* type)
  {
    type_constructor_ = type;
  }
} // namespace ast
