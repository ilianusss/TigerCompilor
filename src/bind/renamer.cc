/**
 ** \file bind/renamer.cc
 ** \brief Implementation of bind::Renamer.
 */

#include <bind/renamer.hh>

namespace bind
{
  using namespace ast;

  // FIXED: Constructor implementation added
  Renamer::Renamer()
    : unique_number_(0)
  {}

  void Renamer::operator()(ast::FunctionDec& e)
  {
    if (e.body_get() != nullptr && e.name_get() != misc::symbol("_main"))
      // not a primitive and not main_ function
      {
        std::string old_name = e.name_get().get();
        std::string new_name =
          old_name + "_" + std::to_string(unique_number_++);
        e.name_set(misc::symbol(new_name));
      }

    super_type::operator()(e);
  }

  void Renamer::operator()(ast::TypeDec& e)
  {
    std::string old_name = e.name_get().get();
    std::string new_name = old_name + "_" + std::to_string(unique_number_++);
    e.name_set(misc::symbol(new_name));

    super_type::operator()(e);
  }

  void Renamer::operator()(ast::VarDec& e)
  {
    std::string old_name = e.name_get().get();
    std::string new_name = old_name + "_" + std::to_string(unique_number_++);
    e.name_set(misc::symbol(new_name));

    super_type::operator()(e);
  }

  void Renamer::operator()(ast::CallExp& e)
  {
    if (e.def_get() != nullptr)
      {
        misc::symbol new_name_bounded = e.def_get()->name_get();
        e.name_set(new_name_bounded);
      }

    super_type::operator()(e);
  }

  void Renamer::operator()(ast::NameTy& e)
  {
    if (e.def_get() != nullptr)
      {
        misc::symbol new_name_bounded = e.def_get()->name_get();
        e.name_set(new_name_bounded);
      }
  }

  void Renamer::operator()(ast::SimpleVar& e)
  {
    if (e.def_get() != nullptr)
      {
        misc::symbol new_name_bounded = e.def_get()->name_get();
        e.name_set(new_name_bounded);
      }
  }

} // namespace bind
