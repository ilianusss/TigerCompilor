#include <ast/all.hh>
#include <ast/default-visitor.hh>
#include <ast/non-object-visitor.hh>
#include <llvmtranslate/escapes-collector.hh>
#include "ast/function-dec.hh"
#include "type/function.hh"

namespace llvmtranslate
{
  /// LLVM IR doesn't support static link and nested functions.
  /// In order to translate those functions to LLVM IR, we use a technique
  /// called Lambda Lifting, which consists in passing a pointer to
  /// the escaped variables to the nested function using that variable.
  ///
  /// In order to do that, we need a visitor to collect these kind of
  /// variables and associate them to each function.

  class EscapesCollector
    : public ast::DefaultConstVisitor
    , public ast::NonObjectConstVisitor
  {
  public:
    /// Super class.
    using super_type = ast::DefaultConstVisitor;
    /// Import overloaded operator() methods.
    using super_type::operator();

    EscapesCollector()
      : did_modify_{false}
      , escaped_{}
      , functionType_stack_{}
    {}

    escaped_map_type& escaped_get() { return escaped_; }

    void operator()(const ast::FunctionChunk& e) override
    {
      bool saved_did_modify = did_modify_;

      // Iterate on the chunk in order to iteratively collect all the callee
      // functions' escaped variables.
      did_modify_ = !e.empty();
      while (did_modify_)
        {
          did_modify_ = false;
          super_type::operator()(e);
        }

      did_modify_ = saved_did_modify;
    }

    void operator()(const ast::FunctionDec& e) override
    {
      // Keep track of the current function
      // FIXED: Some code was deleted here.
      auto* function_type = dynamic_cast<const type::Function*>(e.get_type());
      functionType_stack_.push_back(function_type);

      super_type::operator()(e);

      functionType_stack_.pop_back();
    }

    void operator()(const ast::CallExp& e) override
    {
      super_type::operator()(e);

      // FIXED: Some code was deleted here.
      const type::Function* current_type = functionType_stack_.back();

      // Check whether there are any newly collected escaped variables.
      // If there are, mark the iteration as modified.
      // FIXED: Some code was deleted here.

      const type::Function* called_type =
        dynamic_cast<const type::Function*>(e.def_get()->get_type());
      for (auto* escaped : escaped_[called_type])
        {
          auto& set = escaped_[current_type];
          if (set.insert(escaped).second)
            did_modify_ = true;
        }
    }

    void operator()(const ast::SimpleVar& e) override
    {
      // Associate escaped variables declared in parent frames with their
      // functions
      // FIXED: Some code was deleted here.

      const type::Function* current_type = functionType_stack_.back();

      if (e.def_get()->is_escapable_get())
        {
          auto& set = escaped_[current_type];
          if (set.insert(e.def_get()).second)
            did_modify_ = true;
        }

      super_type::operator()(e);
    }

  private:
    /// Whether any modification was done during the iteration.
    bool did_modify_ = false;

    /// Associate a set of variables with their function.
    escaped_map_type escaped_;

    /// Current visiting function.
    std::vector<const type::Function*> functionType_stack_{};
  };

  escaped_map_type collect_escapes(const ast::Ast& ast)
  {
    EscapesCollector collect;
    collect(ast);

    return std::move(collect.escaped_get());
  }

} // namespace llvmtranslate
