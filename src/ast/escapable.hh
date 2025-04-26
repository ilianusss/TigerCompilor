/**
 ** \file ast/escapable.hh
 ** \brief Declaration of ast::Escapable.
 */

#pragma once

#include <ast/fwd.hh>
#include <misc/xalloc.hh>

namespace ast
{
  /// Escapable.
  class Escapable
  {
    // FIXED: Boolean attribute added + getter/setter + constructor
  public:
    Escapable();

    ~Escapable() = default;

    bool is_escapable_get() const;
    void is_escapable_set(bool is_escapable);

  protected:
    bool is_escapable_;
  };
} // namespace ast
#include <ast/escapable.hxx>
