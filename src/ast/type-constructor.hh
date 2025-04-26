/**
 ** \file ast/type-constructor.hh
 ** \brief Declaration of ast::TypeConstructor.
 */

#pragma once

#include <ast/fwd.hh>
#include <type/fwd.hh>

namespace ast
{
  /** \class ast::TypeConstructor
   ** \brief Create a new type.
   */

  class TypeConstructor
  {
  public:
    TypeConstructor();
    virtual ~TypeConstructor();

    const type::Type* get_type_constructor() const;
    void set_type_constructor(const type::Type* type);

  private:
    const type::Type* type_constructor_;
  };
} // namespace ast
#include <ast/type-constructor.hxx>
