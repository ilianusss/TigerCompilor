/**
 ** \file ast/break-exp.hxx
 ** \brief Inline methods of ast::BreakExp.
 */

#pragma once

#include <ast/break-exp.hh>

// Hint: this needs to be done at TC-3.

namespace ast
{

  // FIXED: Accessors implemented
  inline Exp* BreakExp::def_get() const { return def_; }
  inline void BreakExp::def_set(Exp* def) { def_ = def; }

} // namespace ast
