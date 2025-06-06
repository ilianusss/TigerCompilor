/**
 ** \file bind/binder.hh
 ** \brief Declaration of bind::Binder.
 **/

#pragma once

#include <unordered_map>

#include <ast/default-visitor.hh>
#include <ast/object-visitor.hh>

#include <misc/error.hh>
#include <misc/fwd.hh>
#include <misc/scoped-map.hh>
#include "ast/fwd.hh"

namespace bind
{
  /** \brief Binding identifier uses to their definitions.
   **
   ** When the \c Binder finds a declaration (of a variable/formal, function,
   ** or type), it keeps a pointer to it.  When it finds a use, it binds it
   ** to its definition, i.e., it annotates it with a pointer to the
   ** declaration.
   **
   ** The \c Binder diagnoses identifier use errors (invalid multiple
   ** definitions, unbound identifiers etc.).
   **
   ** Since identifier bindings depend on scopes, it needs an environment.
   **
   ** In the original Tiger by A. Appel, there are two namespaces: on
   ** the one hand types, and on the other hand functions and variables.
   ** Here, at EPITA, we will use three name spaces: we will allow
   ** variables and functions with the same name.
   **
   ** Moreover, object constructs make use of two additional name
   ** spaces: one for class attributes and one for methods (actually
   ** these two name spaces only live within the scope of a class).
   **
   ** Note that this Binder is mainly doing nothing: it is just
   ** interested in declarations and uses.  To avoid writing
   ** all the methods that `do nothing but walk', it derives
   ** from \c ast::DefaultVisitor.
   **/
  class Binder
    : public ast::DefaultVisitor
    , public ast::ObjectVisitor
  {
  public:
    /// Super class type.
    using super_type = ast::DefaultVisitor;
    /// Import all the overloaded \c operator() methods.
    using super_type::operator();

    /// The error handler.
    const misc::error& error_get() const;

    // FIXED: Constructor and destuctor and getter added
    Binder();

    ~Binder() override = default;

    void check_error_main_undefined();

    /* The visiting methods. */
  public:
    // ---------------- //
    // Visiting /Dec/.  //
    // ---------------- //

    /// \name Type and Function declarations
    /// \{

    /// When traversing a function (or a type) we both have to bind
    /// its body (i.e., we need to enter a new scope and push the
    /// arguments in it), *and* we have to store the function's
    /// declaration in the current scope (so that other functions can
    /// call it).

    /// We first introduce the function's name in the outer
    /// environment so that the function can call itself recursively.
    /// In the mean time, we also check for uniqueness.  Then, as a
    /// second step, we process the contents of all the functions
    /// belonging to the current chunk.

    // FIXED: Visitor methods overrided

    // Nodes with def_ attribute
    void operator()(ast::BreakExp&) override;
    void operator()(ast::CallExp&) override;
    void operator()(ast::NameTy&) override;
    void operator()(ast::SimpleVar&) override;
    //void operator()(ast::RecordExp&) override;

    // Nodes declaring new identifiers
    void operator()(ast::FunctionDec&) override;
    void operator()(ast::TypeDec&) override;
    void operator()(ast::VarDec&) override;

    // Nodes to declare new scopes
    void operator()(ast::LetExp&) override;
    void operator()(ast::ForExp&) override;
    void operator()(ast::WhileExp&) override;

    void operator()(ast::TypeChunk&) override;
    void operator()(ast::FunctionChunk&) override;
    void operator()(ast::VarChunk&) override;

  protected:
    /// Binding errors handler.
    misc::error error_;

    // FIXED: member scoped_map and loop_stack_ added
    misc::scoped_map<misc::symbol, ast::FunctionDec*> scoped_map_fun_;
    misc::scoped_map<misc::symbol, ast::VarDec*> scoped_map_var_;
    misc::scoped_map<misc::symbol, ast::TypeDec*> scoped_map_typ_;
    std::vector<ast::Exp*> loop_stack_;
    bool is_main_defined_;
    bool in_loop_;
    bool in_formals_;
    bool in_vardec_;
  };

} // namespace bind

#include <bind/binder.hxx>
