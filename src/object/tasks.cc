/**
 ** \file object/tasks.cc
 ** \brief Object module related tasks' implementation.
 */

#include <memory>

#include <ast/tasks.hh>
#include <common.hh>
#include <object/libobject.hh>
#define DEFINE_TASKS 1
#include <object/tasks.hh>
#undef DEFINE_TASKS

namespace object::tasks
{
  void object_parse() {}

  // FIXME: Some code was deleted here.

  void object_types_compute()
  {
    task_error() << ::object::types_check(*ast::tasks::the_program)
                 << &misc::error::exit_on_error;
  }

  static std::unique_ptr<class_names_type> class_names;

  void object_rename()
  {
    class_names.reset(::object::rename(*ast::tasks::the_program));
  }

} // namespace object::tasks
