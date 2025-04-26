/**
 ** \file bind/tasks.hh
 ** \brief Bind module related tasks.
 */

// FIXED: New tasks linked to binding declared

#pragma once

#include <task/libtask.hh>

namespace bind::tasks
{
  TASK_GROUP("2. Bindings");

  TASK_DECLARE("b|bindings-compute",
               "compute bindings",
               bindings_compute,
               "parse");

  TASK_DECLARE("B|bindings-display",
               "enable bindings display in the AST",
               bindings_display,
               "parse");

  TASK_DECLARE("bound", "bound AST", bound, "bindings-compute");

  TASK_DECLARE("rename",
               "rename identifiers to be unique",
               rename,
               "bindings-compute");

} // namespace bind::tasks
