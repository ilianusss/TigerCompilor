/**
 ** \file bind/tasks.cc
 ** \brief Bind module tasks implementation.
 */

// FIXED: New tasks linked to binding implemented

#include <ostream>

#include <ast/libast.hh>
#include <ast/tasks.hh>
#include <bind/libbind.hh>
#define DEFINE_TASKS 1
#include <bind/tasks.hh>
#undef DEFINE_TASKS
#include <misc/xalloc.hh>

namespace bind::tasks
{
  void bindings_compute() { bind::bindings_compute(*ast::tasks::the_program); }

  void bindings_display() { ast::bindings_display(std::cout) = true; }

  void bound() { /* Return the bound AST implicitly */ }

  void rename() { bind::rename(*ast::tasks::the_program); }

} // namespace bind::tasks
