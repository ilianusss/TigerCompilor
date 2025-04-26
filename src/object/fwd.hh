/**
 ** \file object/fwd.hh
 ** \brief Forward declarations of using types.
 */

#pragma once

#include <misc/map.hh>
#include <misc/symbol.hh>
#include <type/class.hh>

namespace object
{
  /// Names associated to class types.
  using class_names_type = misc::map<const type::Class*, misc::symbol>;
} // namespace object
