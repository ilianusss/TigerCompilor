/**
 ** \file object/tasks.hh
 ** \brief Object module related tasks.
 */

#pragma once

#include <task/libtask.hh>

namespace object::tasks
{
  TASK_GROUP("Object");

  /// Enable object extensions.
  BOOLEAN_TASK_DECLARE("o|object",
                       "enable object extensions",
                       enable_object_extensions_p,
                       "");

  /// Parse the input file, allowing objects.
  TASK_DECLARE("object-parse",
               "parse a file, allowing objects",
               object_parse,
               "object parse");

  // FIXME: Some code was deleted here.

  /// Check for type violation, allowing objects.
  TASK_DECLARE("object-types-compute",
               "check for type violations, "
               "allowing objects",
               object_types_compute,
               "object-bindings-compute");

  /// Perform a renaming, before desugaring objects.
  TASK_DECLARE("object-rename",
               "rename identifiers to unique names, allowing objects",
               object_rename,
               "object-types-compute");

} // namespace object::tasks
