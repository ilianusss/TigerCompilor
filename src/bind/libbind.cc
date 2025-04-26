/**
 ** \file bind/libbind.cc
 ** \brief Define exported bind functions.
 */

// FIXED: Binder + renamer functions implemented + imports added

#include <ast/all.hh>
#include <bind/binder.hh>
#include <bind/libbind.hh>
#include <bind/renamer.hh>
#include <misc/xalloc.hh>

namespace bind
{
  void bindings_compute(ast::Ast& tree)
  {
    Binder binder;
    tree.accept(binder);
    binder.check_error_main_undefined();
    binder.error_get().exit_on_error();
  }

  void rename(ast::Ast& tree)
  {
    Renamer renamer;
    tree.accept(renamer);
  }

} // namespace bind
