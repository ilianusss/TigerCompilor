/**
 ** \file bind/libbind.hh
 ** \brief Interface of the bind module.
 */

// FIXED: Binder + renamer functions declared

#pragma once

#include <ast/fwd.hh>

namespace bind
{
  void bindings_compute(ast::Ast& tree);
  void rename(ast::Ast& tree);

} // namespace bind
