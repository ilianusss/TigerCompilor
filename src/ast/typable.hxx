/**
 ** \file ast/typable.hxx
 ** \brief Inline methods of ast::Typable.
 */

#pragma once

#include <ast/typable.hh>

namespace ast
{
  // FIXED
  inline const type::Type* Typable::get_type() const { return type_; }
  inline void Typable::set_type(const type::Type* type) { type_ = type; }
} // namespace ast
