/**
 ** \file type/type-checker.cc
 ** \brief Implementation for type/type-checker.hh.
 */

#include <iostream>
#include <memory>
#include <ranges>

#include <ast/all.hh>
#include <type/type-checker.hh>
#include <type/types.hh>

namespace type
{
  TypeChecker::TypeChecker()
    : super_type()
    , error_()
  {}

  const Type* TypeChecker::type(ast::Typable& e)
  {
    // FIXED
    if (!e.get_type())
      {
        // Use accept mechanism when available
        if (auto* simple_var = dynamic_cast<ast::SimpleVar*>(&e))
          (*this)(*simple_var);
        else if (auto* field_var = dynamic_cast<ast::FieldVar*>(&e))
          (*this)(*field_var);
        else if (auto* subscript_var = dynamic_cast<ast::SubscriptVar*>(&e))
          (*this)(*subscript_var);
        else if (auto* nil_exp = dynamic_cast<ast::NilExp*>(&e))
          (*this)(*nil_exp);
        else if (auto* int_exp = dynamic_cast<ast::IntExp*>(&e))
          (*this)(*int_exp);
        else if (auto* string_exp = dynamic_cast<ast::StringExp*>(&e))
          (*this)(*string_exp);
        else if (auto* record_exp = dynamic_cast<ast::RecordExp*>(&e))
          (*this)(*record_exp);
        else if (auto* op_exp = dynamic_cast<ast::OpExp*>(&e))
          (*this)(*op_exp);
        else if (auto* call_exp = dynamic_cast<ast::CallExp*>(&e))
          (*this)(*call_exp);
        else if (auto* break_exp = dynamic_cast<ast::BreakExp*>(&e))
          (*this)(*break_exp);
        else if (auto* if_exp = dynamic_cast<ast::IfExp*>(&e))
          (*this)(*if_exp);
        else if (auto* while_exp = dynamic_cast<ast::WhileExp*>(&e))
          (*this)(*while_exp);
        else if (auto* for_exp = dynamic_cast<ast::ForExp*>(&e))
          (*this)(*for_exp);
        else if (auto* let_exp = dynamic_cast<ast::LetExp*>(&e))
          (*this)(*let_exp);
        else if (auto* seq_exp = dynamic_cast<ast::SeqExp*>(&e))
          (*this)(*seq_exp);
        else if (auto* assign_exp = dynamic_cast<ast::AssignExp*>(&e))
          (*this)(*assign_exp);
        else if (auto* array_exp = dynamic_cast<ast::ArrayExp*>(&e))
          (*this)(*array_exp);
        else if (auto* var_dec = dynamic_cast<ast::VarDec*>(&e))
          (*this)(*var_dec);
        else if (auto* name_ty = dynamic_cast<ast::NameTy*>(&e))
          (*this)(*name_ty);
        else if (auto* record_ty = dynamic_cast<ast::RecordTy*>(&e))
          (*this)(*record_ty);
        else if (auto* array_ty = dynamic_cast<ast::ArrayTy*>(&e))
          (*this)(*array_ty);
        else
          // Type not handled, set to void
          e.set_type(&Void::instance());

        assert(e.get_type());
      }
    return e.get_type();
  }

  const Record* TypeChecker::type(const ast::fields_type& e)
  {
    auto res = new Record;
    std::set<misc::symbol> fields;
    // FIXED
    for (const auto& field : e)
      {
        if (fields.find(field->name_get()) != fields.end())
          error(*field, field->name_get().get() + " is duplicate");
        else
          fields.insert(field->name_get());
        res->field_add(field->name_get(), *type(field->type_name_get()));
      }
    return res;
  }

  const Record* TypeChecker::type(const ast::VarChunk& e)
  {
    auto res = new Record;
    for (const auto& var : e)
      res->field_add(var->name_get(), *type(*var));

    return res;
  }

  const misc::error& TypeChecker::error_get() const { return error_; }

  /*-----------------.
  | Error handling.  |
  `-----------------*/

  void TypeChecker::error(const ast::Ast& ast, const std::string& msg)
  {
    error_ << misc::error::error_type::type << ast.location_get() << ": " << msg
           << std::endl;
  }

  void TypeChecker::type_mismatch(const ast::Ast& ast,
                                  const std::string& exp1,
                                  const Type& type1,
                                  const std::string& exp2,
                                  const Type& type2)
  {
    error_ << misc::error::error_type::type << ast.location_get()
           << ": type mismatch" << misc::incendl << exp1 << " type: " << type1
           << misc::iendl << exp2 << " type: " << type2 << misc::decendl;
  }

  void TypeChecker::check_types(const ast::Ast& ast,
                                const std::string& exp1,
                                const Type& type1,
                                const std::string& exp2,
                                const Type& type2)
  {
    // FIXED
    if (!type1.compatible_with(type2))
      {
        type_mismatch(ast, exp1, type1, exp2, type2);
        return;
      }

    // If any of the type is Nil, set its `record_type_` to the other type.
    if (!error_)
      {
        // FIXED
        if (const Nil* nil1 = dynamic_cast<const Nil*>(&type1.actual()))
          nil1->record_type_set(type2.actual());
        if (const Nil* nil2 = dynamic_cast<const Nil*>(&type2.actual()))
          nil2->record_type_set(type1.actual());
      }
  }

  void TypeChecker::check_types(const ast::Ast& ast,
                                const std::string& exp1,
                                ast::Typable& type1,
                                const std::string& exp2,
                                ast::Typable& type2)
  {
    // Ensure evaluation order.
    type(type1);
    type(type2);
    // FIXED
    check_types(ast, exp1, *type1.get_type(), exp2, *type2.get_type());
  }

  /*--------------------------.
  | The core of the visitor.  |
  `--------------------------*/

  /*-----------------.
  | Visiting /Var/.  |
  `-----------------*/

  void TypeChecker::operator()(ast::SimpleVar& e)
  {
    // FIXED
    type_default(e, e.def_get()->get_type());
  }

  void TypeChecker::operator()(ast::FieldVar& e)
  {
    // FIXED
    const Record* record =
      dynamic_cast<const Record*>(&type(e.var_get())->actual());
    if (!record)
      {
        error(e, "not a record");
        e.set_type(&Void::instance());
        return;
      }
    if (record->field_index(e.name_get()) < 0)
      {
        error(e, e.name_get().get() + " is not a field of the record");
        e.set_type(&Void::instance());
        return;
      }
    type_default(e, record->field_type(e.name_get()));
    if (!e.get_type())
      e.set_type(&Void::instance());
  }

  void TypeChecker::operator()(ast::SubscriptVar& e)
  {
    // FIXED
    type(e.var_get());
    type(e.index_get());
    check_type(e.index_get(), "index is not an int", Int::instance());

    const Array* array =
      dynamic_cast<const Array*>(&e.var_get().get_type()->actual());
    if (!array)
      {
        error(e, "not an array");
        e.set_type(&Void::instance());
      }
    else
      type_default(e, &array->type_get());

    if (!e.get_type())
      e.set_type(&Void::instance());
  }

  /*-----------------.
  | Visiting /Exp/.  |
  `-----------------*/

  // Literals.
  void TypeChecker::operator()(ast::NilExp& e)
  {
    auto nil_ptr = std::make_unique<Nil>();
    type_default(e, nil_ptr.get());
    created_type_default(e, nil_ptr.release());
  }

  void TypeChecker::operator()(ast::IntExp& e)
  {
    // FIXED
    type_default(e, &Int::instance());
  }

  void TypeChecker::operator()(ast::StringExp& e)
  {
    // FIXED
    type_default(e, &String::instance());
  }

  // Complex values.

  void TypeChecker::operator()(ast::RecordExp& e)
  {
    // FIXED
    type(e.type_name_get());
    const Record* record =
      dynamic_cast<const Record*>(&e.type_name_get().get_type()->actual());

    if (!record)
      {
        error(e, e.type_name_get().name_get().get() + " is not a record type");
        e.set_type(&Void::instance());
        return;
      }

    for (const auto& field : e.fields_get())
      {
        type(field->init_get());
        const auto* field_type = record->field_type(field->name_get());
        if (!field_type)
          error(*field,
                "field " + field->name_get().get() + " not found in record "
                  + e.type_name_get().name_get().get());
        else
          check_types(*field, "init", *field->init_get().get_type(),
                      "field type", *field_type);
      }

    type_default(e, e.type_name_get().get_type());

    if (!e.get_type())
      e.set_type(&Void::instance());
  }

  void TypeChecker::operator()(ast::OpExp& e)
  {
    // FIXED
    type(e.left_get());
    type(e.right_get());

    if (e.oper_get() == ast::OpExp::Oper::add
        || e.oper_get() == ast::OpExp::Oper::sub
        || e.oper_get() == ast::OpExp::Oper::mul
        || e.oper_get() == ast::OpExp::Oper::div)
      {
        check_type(e.left_get(), "left operand must be an integer",
                   Int::instance());
        check_type(e.right_get(), "right operand must be an integer",
                   Int::instance());
        type_default(e, &Int::instance());
      }
    else if (e.oper_get() == ast::OpExp::Oper::eq
             || e.oper_get() == ast::OpExp::Oper::ne)
      {
        check_types(e, "left", e.left_get(), "right", e.right_get());
        type_default(e, &Int::instance());
      }
    else if (e.oper_get() == ast::OpExp::Oper::lt
             || e.oper_get() == ast::OpExp::Oper::le
             || e.oper_get() == ast::OpExp::Oper::gt
             || e.oper_get() == ast::OpExp::Oper::ge)
      {
        check_types(e, "left", e.left_get(), "right", e.right_get());
        type_default(e, &Int::instance());
      }
    else
      {
        e.set_type(&Void::instance());
        assert(false);
      }

    if (!e.get_type())
      e.set_type(&Int::instance());
  }

  // FIXED
  void TypeChecker::operator()(ast::CallExp& e)
  {
    const Function* function =
      dynamic_cast<const Function*>(e.def_get()->get_type());
    if (!function)
      {
        error(e, e.name_get().get() + " is not a function");
        e.set_type(&Void::instance());
        return;
      }

    if (function->formals_get().fields_get().size() > e.args_get().size())
      error(e, e.name_get().get() + " has too few arguments");
    if (function->formals_get().fields_get().size() < e.args_get().size())
      error(e, e.name_get().get() + " has too many arguments");

    for (size_t i = 0;
         !error_get() && i < function->formals_get().fields_get().size(); i++)
      {
        auto exp = dynamic_cast<ast::Exp*>(e.args_get()[i]);
        type(*exp);
        check_types(e, "parameter",
                    function->formals_get().fields_get()[i].type_get(),
                    "argument", *e.args_get()[i]->get_type());
      }

    type_default(e, &function->result_get());

    if (!e.get_type())
      e.set_type(&Void::instance());
  }

  void TypeChecker::operator()(ast::BreakExp& e)
  {
    type_default(e, &Void::instance());
  }

  void TypeChecker::operator()(ast::IfExp& e)
  {
    // FIXED
    type(e.test_get());
    auto& test = e.test_get();

    check_type(test, "test is not an integer", Int::instance());

    // Get the clauses and check for null before using them
    auto* then = &e.thenclause_get();
    if (then)
      type(*then);
    else
      {
        e.set_type(&Void::instance());
        return;
      }

    auto* else_ = &e.elseclause_get();
    bool has_else = (else_ != nullptr);
    if (else_)
      type(*else_);

    // If there's no else clause, then clause must be void
    if (!has_else && then && then->get_type()
        && !dynamic_cast<const Void*>(then->get_type()))
      error(e, "if-then returns non unit");

    if (then && else_)
      check_types(e, "then", *then, "else", *else_);

    // Set the type based on the clauses
    if (then && then->get_type() && dynamic_cast<const Nil*>(then->get_type()))
      {
        if (else_ && else_->get_type())
          e.set_type(else_->get_type());
        else
          e.set_type(&Void::instance());
      }
    else if (then && then->get_type())
      e.set_type(then->get_type());
    else
      e.set_type(&Void::instance());
  }

  void TypeChecker::operator()(ast::WhileExp& e)
  {
    // FIXED
    type(e.test_get());
    type(e.body_get());
    check_type(e.test_get(), "test is not an integer", Int::instance());
    check_type(e.body_get(), "body must be void", Void::instance());

    type_default(e, &Void::instance());

    if (!e.get_type())
      e.set_type(&Void::instance());
  }

  void TypeChecker::operator()(ast::ForExp& e)
  {
    type(e.vardec_get());
    check_type(e.vardec_get(), "index variable must be an integer",
               Int::instance());

    type(e.hi_get());
    check_type(e.hi_get(), "high bound must be an integer", Int::instance());

    // Mark the loop variable as read-only before processing the body
    var_read_only_.insert(&e.vardec_get());

    type(e.body_get());
    check_type(e.body_get(), "body must be void", Void::instance());

    // Remove the loop variable from read-only set after processing
    var_read_only_.erase(&e.vardec_get());

    type_default(e, &Void::instance());

    if (!e.get_type())
      e.set_type(&Void::instance());
  }

  void TypeChecker::operator()(ast::LetExp& e)
  {
    for (const auto& chunk : e.chunks_get())
      (*this)(*chunk);

    type(e.body_get());
    type_default(e, e.body_get().get_type());

    if (!e.get_type())
      e.set_type(&Void::instance());
  }

  void TypeChecker::operator()(ast::SeqExp& e)
  {
    if (e.exps_get().empty())
      {
        type_default(e, &Void::instance());
        return;
      }

    for (const auto& exp : e.exps_get())
      type(*exp);

    type_default(e, e.exps_get().back()->get_type());

    if (!e.get_type())
      e.set_type(&Void::instance());
  }

  void TypeChecker::operator()(ast::AssignExp& e)
  {
    type(e.var_get());
    type(e.exp_get());

    check_types(e, "variable", *e.var_get().get_type(), "expression",
                *e.exp_get().get_type());

    // Check if the variable is a for-loop index (read-only)
    auto* simple_var = dynamic_cast<ast::SimpleVar*>(&e.var_get());
    if (simple_var && simple_var->def_get()
        && var_read_only_.find(simple_var->def_get()) != var_read_only_.end())
      error(e, "index variable erroneously assigned to");

    type_default(e, &Void::instance());

    if (!e.get_type())
      e.set_type(&Void::instance());
  }

  void TypeChecker::operator()(ast::ArrayExp& e)
  {
    type(e.type_name_get());
    const Array* array =
      dynamic_cast<const Array*>(&e.type_name_get().get_type()->actual());

    if (!array)
      {
        error(e, e.type_name_get().name_get().get() + " is not an array type");
        e.set_type(&Void::instance());
        return;
      }

    type(e.size_get());
    check_type(e.size_get(), "size must be an integer", Int::instance());

    type(e.init_get());
    check_types(e, "element type", array->type_get(), "init",
                *e.init_get().get_type());

    type_default(e, array);

    if (!e.get_type())
      e.set_type(&Void::instance());
  }

  /*-----------------.
  | Visiting /Dec/.  |
  `-----------------*/

  /*------------------------.
  | Visiting FunctionChunk. |
  `------------------------*/

  void TypeChecker::operator()(ast::FunctionChunk& e)
  {
    chunk_visit<ast::FunctionDec>(e);
  }

  void TypeChecker::operator()(ast::FunctionDec&)
  {
    // We must not be here.
    unreachable();
  }

  // Store the type of this function.
  template <>
  void TypeChecker::visit_dec_header<ast::FunctionDec>(ast::FunctionDec& e)
  {
    // FIXED
    auto formals = new Record;
    for (const auto& formal : e.formals_get())
      formals->field_add(formal->name_get(), *type(*formal));

    const Type* result = &Void::instance();
    if (e.result_get())
      result = type(*e.result_get());

    auto function = new Function(formals, *result);
    type_default(e, function);
  }

  // Type check this function's body.
  template <>
  void TypeChecker::visit_dec_body<ast::FunctionDec>(ast::FunctionDec& e)
  {
    if (e.body_get())
      visit_routine_body<Function>(e);
  }

  /*---------------.
  | Visit VarDec.  |
  `---------------*/

  void TypeChecker::operator()(ast::VarDec& e)
  {
    // FIXED
    if (e.init_get())
      {
        type(*e.init_get());

        if (dynamic_cast<const Nil*>(e.init_get()->get_type())
            && !e.type_name_get())
          {
            error(e, "cannot initialize with nil without type declaration");
            type_default(e, &Void::instance());
          }

        if (e.type_name_get())
          {
            type_default(e, type(*e.type_name_get()));
            check_types(e, "type name", *e.type_name_get()->get_type(), "init",
                        *e.init_get()->get_type());
          }
        else
          type_default(e, e.init_get()->get_type());
      }
    else if (e.type_name_get())
      type_default(e, type(*e.type_name_get()));
    else
      type_default(e, &Void::instance());

    if (!e.get_type())
      e.set_type(&Void::instance());
  }

  /*--------------------.
  | Visiting TypeChunk. |
  `--------------------*/

  void TypeChecker::operator()(ast::TypeChunk& e)
  {
    chunk_visit<ast::TypeDec>(e);
  }

  void TypeChecker::operator()(ast::TypeDec&)
  {
    // We must not be here.
    unreachable();
  }

  // Store this type.
  template <> void TypeChecker::visit_dec_header<ast::TypeDec>(ast::TypeDec& e)
  {
    // We only process the head of the type declaration, to set its
    // name in E.  A declaration has no type in itself; here we store
    // the type declared by E.
    // FIXED
    Named* ty = new Named(e.name_get());
    created_type_default(e, ty);
    type_default(e, ty);
  }

  // Bind the type body to its name.
  template <> void TypeChecker::visit_dec_body<ast::TypeDec>(ast::TypeDec& e)
  {
    // FIXED
    const Named* name = dynamic_cast<const Named*>(e.get_type());
    name->type_set(type(e.ty_get()));
    if (!name->sound())
      {
        error(e, "recursive type not allowed");
        name->type_set(&Void::instance());
      }
  }

  /*------------------.
  | Visiting /Chunk/. |
  `------------------*/

  template <class D> void TypeChecker::chunk_visit(ast::Chunk<D>& e)
  {
    // FIXED
    for (auto& dec : e.decs_get())
      visit_dec_header(*dec);
    for (auto& dec : e.decs_get())
      visit_dec_body(*dec);
  }

  /*-------------.
  | Visit /Ty/.  |
  `-------------*/

  void TypeChecker::operator()(ast::NameTy& e)
  {
    // FIXED
    if (e.name_get() == "int")
      type_default(e, &Int::instance());
    else if (e.name_get() == "string")
      type_default(e, &String::instance());
    else
      {
        ast::TypeDec* ty = dynamic_cast<ast::TypeDec*>(e.def_get());
        type_default(e, ty->get_type());
      }
  }

  void TypeChecker::operator()(ast::RecordTy& e)
  {
    // FIXED
    const Record* record = type(e.fields_get());
    e.set_type_constructor(const_cast<Record*>(record));
    e.set_type(record);
  }

  void TypeChecker::operator()(ast::ArrayTy& e)
  {
    // FIXED
    type(e.base_type_get());
    type_default(e, new Array(*e.base_type_get().get_type()));
  }

} // namespace type
