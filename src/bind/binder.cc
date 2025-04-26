/**
 ** \file bind/binder.cc
 ** \brief Implementation for bind/binder.hh.
 */

#include <exception>
#include <ast/all.hh>
#include <bind/binder.hh>

#include <misc/contract.hh>
#include "ast/ast.hh"

namespace bind
{
  /*-----------------.
  | Error handling.  |
  `-----------------*/

  /// The error handler.
  const misc::error& Binder::error_get() const { return error_; }

  // FIXED: Constructor implementation added
  Binder::Binder()
    : scoped_map_fun_()
    , scoped_map_var_()
    , scoped_map_typ_()
    , loop_stack_()
    , is_main_defined_(false)
    , in_loop_(false)
    , in_formals_(false)
    , in_vardec_(false)
  {}

  void Binder::check_error_main_undefined()
  {
    if (is_main_defined_ == false)
      error_ << misc::error::error_type::bind
             << "invalid: _main function never defined\n";
  }

  using namespace ast;
  /*
  // FIXED: Some operators implementations added
  */

  void Binder::operator()(BreakExp& e)
  {
    if (loop_stack_.empty())
      {
        error_ << misc::error::error_type::bind << e.location_get() << ": "
               << ": `break` outside any loop\n";
        return;
      }
    if (in_loop_)
      {
        error_ << misc::error::error_type::bind << e.location_get() << ": "
               << ": `break` in condition of a loop\n";
        return;
      }
    if (in_vardec_)
      {
        error_ << misc::error::error_type::bind << e.location_get() << ": "
               << ": `break` in var declaration\n";
        return;
      }
    if (in_formals_)
      {
        error_ << misc::error::error_type::bind << e.location_get() << ": "
               << ": `break` in function call\n";
        return;
      }
    ast::Exp* def = loop_stack_.back();
    e.def_set(def);
  }

  void Binder::operator()(CallExp& e)
  {
    misc::symbol identifier = e.name_get();
    try
      {
        ast::FunctionDec* def =
          static_cast<ast::FunctionDec*>(scoped_map_fun_.get(identifier));
        e.def_set(def);
      }
    catch (std::exception& excp)
      {
        error_ << misc::error::error_type::bind << e.location_get() << ": "
               << "undeclared function: " << identifier << "\n";
      }
    in_formals_ = true;
    for (const auto& arg : e.args_get())
      arg->accept(*this);
    in_formals_ = false;
  }

  void Binder::operator()(NameTy& e)
  {
    misc::symbol identifier = e.name_get();
    if (identifier != misc::symbol("string")
        && identifier != misc::symbol("int"))
      {
        try
          {
            ast::TypeDec* def =
              static_cast<ast::TypeDec*>(scoped_map_typ_.get(identifier));
            e.def_set(def);
          }
        catch (std::exception& excp)
          {
            error_ << misc::error::error_type::bind << e.location_get() << ": "
                   << "undeclared type: " << identifier << "\n";
          }
      }
  }

  void Binder::operator()(SimpleVar& e)
  {
    misc::symbol identifier = e.name_get();
    try
      {
        ast::VarDec* def =
          static_cast<ast::VarDec*>(scoped_map_var_.get(identifier));
        e.def_set(def);
      }
    catch (std::exception& excp)
      {
        error_ << misc::error::error_type::bind << e.location_get() << ": "
               << "undeclared variable: " << identifier << "\n";
      }
  }

  void Binder::operator()(FunctionDec& e)
  {
    misc::symbol identifier = e.name_get();
    if (identifier == misc::symbol("_main"))
      {
        if (is_main_defined_)
          error_ << misc::error::error_type::bind << e.location_get() << ": "
                 << "redeclaration of _main function\n";
        else
          is_main_defined_ = true;
      }

    ast::FunctionDec* node = &e;
    scoped_map_fun_.put(identifier, node);

    scoped_map_fun_.scope_begin();
    scoped_map_var_.scope_begin();

    in_formals_ = true;
    e.formals_get().accept(*this);
    in_formals_ = false;
    if (e.result_get() != nullptr)
      this->accept(e.result_get()); // result type can be null
    if (e.body_get() != nullptr)
      this->accept(e.body_get()); // body can be null for primitive declaration

    scoped_map_fun_.scope_end();
    scoped_map_var_.scope_end();
  }

  void Binder::operator()(TypeDec& e)
  {
    misc::symbol identifier = e.name_get();

    ast::TypeDec* node = &e;
    scoped_map_typ_.put(identifier, node);

    super_type::operator()(e);
  }

  void Binder::operator()(VarDec& e)
  {
    misc::symbol identifier = e.name_get();
    ast::VarDec* node = &e;
    scoped_map_var_.put(identifier, node);

    // `type_name' might be omitted.
    if (e.type_name_get() != nullptr)
      this->accept(e.type_name_get());
    // `init' can be null in case of formal parameter.
    in_vardec_ = true;
    if (e.init_get() != nullptr)
      this->accept(e.init_get());
    in_vardec_ = false;
  }

  void Binder::operator()(LetExp& e)
  {
    scoped_map_fun_.scope_begin();
    scoped_map_var_.scope_begin();
    scoped_map_typ_.scope_begin();

    super_type::operator()(e);

    scoped_map_fun_.scope_end();
    scoped_map_var_.scope_end();
    scoped_map_typ_.scope_end();
  }

  void Binder::operator()(ForExp& e)
  {
    scoped_map_var_.scope_begin();
    ast::Exp* node = &e;
    loop_stack_.push_back(node);

    in_loop_ = true;
    e.vardec_get().accept(*this);
    e.hi_get().accept(*this);
    in_loop_ = false;
    e.body_get().accept(*this);

    scoped_map_var_.scope_end();
    loop_stack_.pop_back();
  }

  void Binder::operator()(WhileExp& e)
  {
    scoped_map_var_.scope_begin();
    ast::Exp* node = &e;
    loop_stack_.push_back(node);

    in_loop_ = true;
    e.test_get().accept(*this);
    in_loop_ = false;
    e.body_get().accept(*this);

    scoped_map_var_.scope_end();
    loop_stack_.pop_back();
  }

  void Binder::operator()(TypeChunk& e)
  {
    std::map<misc::symbol, ast::Ast*> map;
    for (auto it = e.begin(); it != e.end(); ++it)
      {
        misc::symbol identifier = (*it)->name_get();
        auto node = map.find(identifier);
        if (node != map.end())
          {
            error_ << misc::error::error_type::bind << e.location_get() << ": "
                   << "redeclaration of type: " << identifier << "\n"
                   << node->second->location_get() << ": first definition\n";
          }
        else
          {
            map[std::move(identifier)] = *it;
            scoped_map_typ_.put(identifier, *it);
          }
      }
    for (auto it = e.begin(); it != e.end(); ++it)
      (*it)->accept(*this);
  }

  void Binder::operator()(FunctionChunk& e)
  {
    std::map<misc::symbol, ast::Ast*> map;
    for (auto it = e.begin(); it != e.end(); ++it)
      {
        misc::symbol identifier = (*it)->name_get();
        auto node = map.find(identifier);
        if (node != map.end())
          {
            error_ << misc::error::error_type::bind << e.location_get() << ": "
                   << "redeclaration of function: " << identifier << "\n"
                   << node->second->location_get() << ": first definition\n";
          }
        else
          {
            map[std::move(identifier)] = *it;
            scoped_map_fun_.put(identifier, *it);
          }
      }
    for (auto it = e.begin(); it != e.end(); ++it)
      (*it)->accept(*this);
  }

  void Binder::operator()(VarChunk& e)
  {
    std::map<misc::symbol, ast::Ast*> map;
    for (auto it = e.begin(); it != e.end(); ++it)
      {
        misc::symbol identifier = (*it)->name_get();
        auto node = map.find(identifier);
        if (node != map.end())
          {
            error_ << misc::error::error_type::bind << e.location_get() << ": "
                   << "redeclaration of formal: " << identifier << "\n"
                   << node->second->location_get() << ": first definition\n";
          }
        else
          {
            map[std::move(identifier)] = *it;
          }
      }
    for (auto it = e.begin(); it != e.end(); ++it)
      (*it)->accept(*this);
  }

} // namespace bind
