/**
 ** \file object/libobject.hh
 ** \brief Declare functions and variables exported by object module.
 */

#pragma once

#include <ast/fwd.hh>
#include <misc/error.hh>
#include <object/fwd.hh>

namespace object
{
  /*-------.
  | Bind.  |
  `-------*/

  // FIXME: Some code was deleted here.

  /*----------------.
  | Compute types.  |
  `----------------*/

  /** \brief Check types allowing objects.

      \param tree abstract syntax tree's root.

      \return success of the type-checking.  */
  misc::error types_check(ast::Ast& tree);

  /*---------.
  | Rename.  |
  `---------*/

  /// Rename the variables of an AST so that they each have a unique
  /// name, with support for objects.
  ///
  /// \param tree  abstract syntax tree's root, whose bindings and types
  ///              have been computed.
  /// \return      a newly allocated dictionnary of class names
  class_names_type* rename(ast::Ast& tree);

} // namespace object

#include <object/libobject.hxx>
