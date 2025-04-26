/**
 ** \file llvmtranslate/translator.cc
 ** \brief Implementation of llvmtranslate::Translator
 */

#include "ast/op-exp.hh"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include <llvm/Config/llvm-config.h> // LLVM_VERSION_*
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Verifier.h> // llvm::verifyFunction
#include <llvm/Support/Casting.h>
#include <llvm/TargetParser/Host.h> // llvm::sys
#include <llvm/TargetParser/Triple.h>

#pragma GCC diagnostic pop

#include <ast/all.hh>
#include <llvmtranslate/translator.hh>

namespace llvmtranslate
{
  using namespace std::string_literals;

  namespace
  {
    // Shorthands for integer type and pointer to integer type.
    inline llvm::IntegerType* i32_t(llvm::LLVMContext& ctx)
    {
      return llvm::Type::getInt32Ty(ctx);
    }

    inline llvm::PointerType* i32p_t(llvm::LLVMContext& ctx)
    {
      return llvm::PointerType::get(llvm::Type::getInt32Ty(ctx), 0);
    }

    llvm::AllocaInst* create_alloca(llvm::Function* ll_function,
                                    llvm::Type* ll_type,
                                    const std::string& name)
    {
      // Create an IRBuilder starting at the beginning of the current entry
      // block. LLVM treats allocas as local variables only if they occur at the
      // beginning of a function.
      llvm::IRBuilder<> tmp(&ll_function->getEntryBlock(),
                            ll_function->getEntryBlock().begin());
      return tmp.CreateAlloca(ll_type, nullptr, name);
    }

    // Set default attributes to the functions
    void set_default_attributes(llvm::Function& the_function,
                                const ast::FunctionDec& e)
    {
      the_function.addFnAttr(llvm::Attribute::NoUnwind); // No exceptions in TC
      if (!e.body_get())                                 // Inline primitives
        the_function.addFnAttr(llvm::Attribute::InlineHint);
    }

    std::string function_dec_name(const ast::FunctionDec& e)
    {
      // Rename "_main" to "tc_main"
      if (e.name_get() == "_main")
        return "tc_main";
      // Prefix all the primitives with "tc_"
      if (!e.body_get())
        return "tc_" + e.name_get().get();
      return e.name_get().get();
    }
  } // namespace

  Translator::Translator(llvm::Module& module, escaped_map_type&& escaped)
    : module_{module}
    , ctx_{module_.getContext()}
    , builder_{ctx_}
    , escaped_{std::move(escaped)}
    , type_visitor_{ctx_}
  {
    // The current process triple.
    auto process_triple = llvm::Triple(llvm::sys::getProcessTriple());
    // Set the 32-bit version of the triple.
    module_.setTargetTriple(process_triple.get32BitArchVariant().str());
  }

  void Translator::operator()(const ast::Ast& e)
  {
    translate(e);
    value_ = nullptr;
  }

  llvm::Value* Translator::translate(const ast::Ast& node)
  {
    node.accept(*this);
    return value_;
  }

  llvm::Value* Translator::access_var(const ast::Var& e)
  {
    if (auto var_ast = dynamic_cast<const ast::SimpleVar*>(&e))
      {
        // FIXED: Some code was deleted here.
        const ast::VarDec* var_dec = var_ast->def_get();
        auto& myLocals = locals_[current_function_];
        auto it = myLocals.find(var_dec);
        if (it != myLocals.end())
          return it->second;

        for (auto& kv : locals_)
          {
            auto jt = kv.second.find(var_dec);
            if (jt != kv.second.end())
              return jt->second;
          }
      }
    else if (auto arr_ast = dynamic_cast<const ast::SubscriptVar*>(&e))
      {
        // FIXED: Some code was deleted here.
        llvm::Value* base_ptr = translate(arr_ast->var_get());
        llvm::Value* idx_val = translate(arr_ast->index_get());
        return builder_.CreateInBoundsGEP(i32_t(ctx_), base_ptr, idx_val,
                                          "subscriptptr");
      }
    else if (auto field_ast = dynamic_cast<const ast::FieldVar*>(&e))
      {
        const ast::Var* var = nullptr;
        // FIXED: Some code was deleted here.
        var = &field_ast->var_get();
        auto var_val = translate(*var);

        const type::Record* record_type = nullptr;
        // FIXED: Some code was deleted here.
        const type::Type& sem_type = field_ast->var_get().get_type()->actual();

        record_type = &static_cast<const type::Record&>(sem_type);

        misc::symbol field_name;
        // FIXED: Some code was deleted here.
        field_name = field_ast->name_get();

        int index = -1;
        // FIXED: Some code was deleted here (Get the index of the field).
        index = 0;
        for (const auto& f : record_type->fields_get())
          {
            if (f.name_get() == field_name)
              break;
            ++index;
          }

        // The GEP instruction provides us with safe pointer arithmetics,
        // usually used with records or arrays.
        llvm::Type* record_ltype = nullptr;
        // FIXED: Some code was deleted here (Get record's corresponding LLVM type).
        llvm_type(*record_type);
        record_ltype = type_visitor_.get_record_ltype(record_type);

        return builder_.CreateStructGEP(record_ltype, var_val, index,
                                        "fieldptr_"s + field_name.get());
      }
    else
      unreachable();
    return nullptr;
  }

  llvm::Value* Translator::init_array(llvm::Value* count_val,
                                      llvm::Value* init_val)
  {
    // Cast everything so that it is conform to the signature of init_array
    // int *init_array(int, int)

    // We need to separate the pointers and the ints.
    // LLVM requires a ptrtoint instruction for pointers
    // and a bitcast for others.
    auto init_val_cast = init_val->getType()->isPointerTy()
      ? builder_.CreatePtrToInt(init_val, i32_t(ctx_), "init_array_ptrtoint")
      : builder_.CreateBitCast(init_val, i32_t(ctx_), "init_array_bitcast");

    // Create the init_array function:
    // First, the arguments (int*, int, int)
    std::vector<llvm::Type*> arg_type{i32_t(ctx_), init_val_cast->getType()};

    // Then, create the FunctionType.
    auto init_array_ltype =
      llvm::FunctionType::get(i32p_t(ctx_), arg_type, false);

    // Get the llvm::Function from the module related to the name and type
    auto init_array_function =
      module_.getOrInsertFunction("tc_init_array", init_array_ltype);

    // Prepare the arguments.
    std::vector<llvm::Value*> arg_vals{count_val, init_val_cast};

    // Create the call.
    auto init_array_call =
      builder_.CreateCall(init_array_function, arg_vals, "init_array_call");

    // Cast the result of the call in the desired type.
    return builder_.CreateBitCast(init_array_call,
                                  init_val->getType()->getPointerTo(),
                                  "init_array_call_cast");
  }

  llvm::Type* Translator::llvm_type(const type::Type& type)
  {
    type_visitor_(type);
    return type_visitor_.llvm_type_get();
  }

  llvm::FunctionType*
  Translator::llvm_function_type(const type::Function& function_type)
  {
    // Prepare the arguments
    std::vector<llvm::Type*> args_types;
    // First, if there are any escaped vars, create ptr arguments for it
    // (Lambda lifting)

    if (auto escapes_it = escaped_.find(&function_type);
        escapes_it != std::end(escaped_))
      {
        auto& escapes = escapes_it->second;
        args_types.reserve(escapes.size()
                           + function_type.formals_get().fields_get().size());
        for (const auto dec : escapes)
          {
            llvm::Type* var_ltype = nullptr;
            // FIXED: Some code was deleted here (Get the llvm type of the VarDec).
            var_ltype = llvm_type(*dec->get_type());
            args_types.emplace_back(llvm::PointerType::getUnqual(var_ltype));
          }
      }
    else
      args_types.reserve(function_type.formals_get().fields_get().size());

    // Then, the actual arguments
    for (const auto& field : function_type.formals_get())
      args_types.emplace_back(llvm_type(field.type_get()));

    llvm::Type* result_ltype = nullptr;
    // FIXED: Some code was deleted here (If the result is void typed, we assign llvm void type to result_ltype).
    if (dynamic_cast<const type::Void*>(&function_type.result_get()))
      result_ltype = llvm::Type::getVoidTy(ctx_);
    else
      result_ltype = llvm_type(function_type.result_get());

    return llvm::FunctionType::get(result_ltype, args_types, false);
  }

  void Translator::operator()(const ast::SimpleVar& e)
  {
    // Void var types are actually Ints represented by a 0
    // FIXED: Some code was deleted here.
    llvm::Value* var_ptr = access_var(e);

    llvm::Type* ll_type = llvm_type(*e.get_type());

    if (ll_type->isVoidTy())
      {
        value_ = llvm::ConstantInt::get(i32_t(ctx_), 0);
      }
    else
      {
        value_ = builder_.CreateLoad(ll_type, var_ptr, e.name_get().get());
      }
  }

  void Translator::operator()(const ast::FieldVar& e)
  {
    // FIXED: Some code was deleted here.
    llvm::Value* field_ptr = access_var(e);

    llvm::Type* ll_type = llvm_type(*e.get_type());

    if (ll_type->isVoidTy())
      {
        value_ = llvm::ConstantInt::get(i32_t(ctx_), 0);
      }
    else
      {
        value_ = builder_.CreateLoad(ll_type, field_ptr, e.name_get().get());
      }
  }

  void Translator::operator()(const ast::SubscriptVar& e)
  {
    // FIXED: Some code was deleted here.
    llvm::Value* elm_ptr = access_var(e);

    llvm::Type* ll_type = llvm_type(*e.get_type());

    if (ll_type->isVoidTy())
      {
        value_ = llvm::ConstantInt::get(i32_t(ctx_), 0);
      }
    else
      {
        value_ = builder_.CreateLoad(ll_type, elm_ptr, "arrayelem");
      }
  }

  void Translator::operator()(const ast::NilExp& e)
  {
    // FIXED: Some code was deleted here (Get the record_type of the Nil type, and create a null pointer).
    auto* nilTy = dynamic_cast<const type::Nil*>(e.get_type());

    const type::Type* sem = nilTy->record_type_get();

    while (auto* named = dynamic_cast<const type::Named*>(sem))
      sem = &named->actual();
    auto* record = dynamic_cast<const type::Record*>(sem);

    llvm_type(*record);
    auto* structL =
      llvm::cast<llvm::StructType>(type_visitor_.get_record_ltype(record));

    auto* ptrTy = structL->getPointerTo();
    value_ =
      llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy));
  }

  void Translator::operator()(const ast::IntExp& e)
  {
    // FIXED: Some code was deleted here (Integers in Tiger are all 32bit signed).
    value_ = llvm::ConstantInt::get(i32_t(ctx_), e.value_get(), true);
  }

  void Translator::operator()(const ast::StringExp& e)
  {
    // FIXED: Some code was deleted here (Strings are translated as `i8*` values, like C's `char*`).
    value_ = builder_.CreateGlobalStringPtr(e.value_get(), "str");
  }

  void Translator::operator()(const ast::RecordExp& e)
  {
    // Get the record type
    const type::Record* record_type = nullptr;
    // FIXED: Some code was deleted here.

    const type::Type& sem_type = e.get_type()->actual();
    record_type = &static_cast<const type::Record&>(sem_type);

    // Type the record and use get_record_ltype() to get its LLVM type
    llvm_type(*record_type);
    llvm::Type* rec_ltype = type_visitor_.get_record_ltype(record_type);
    auto* struct_ltype = llvm::cast<llvm::StructType>(rec_ltype);

    // The size of the structure and cast it to int
    auto sizeof_val = llvm::ConstantExpr::getSizeOf(struct_ltype);
    sizeof_val = llvm::ConstantExpr::getTruncOrBitCast(sizeof_val, i32_t(ctx_));

    // Generate the instruction calling Malloc
    auto malloc_val = builder_.CreateMalloc(
      i32_t(ctx_), struct_ltype, sizeof_val, nullptr, nullptr, "malloccall");

    // Init the fields
    // FIXED: Some code was deleted here.
    for (const auto& field : e.fields_get())
      {
        misc::symbol field_name = field->name_get();
        int index = 0;
        for (const auto& f : record_type->fields_get())
          {
            if (f.name_get() == field_name)
              break;
            ++index;
          }
        llvm::Value* field_ptr = builder_.CreateStructGEP(
          struct_ltype, malloc_val, index, "fieldptr_"s + field_name.get());
        llvm::Value* init_val = translate(field->init_get());
        builder_.CreateStore(init_val, field_ptr);
      }

    value_ = malloc_val;
  }

  void Translator::operator()(const ast::OpExp& e)
  {
    // FIXED: Some code was deleted here.
    auto left_val = translate(e.left_get());
    auto right_val = translate(e.right_get());
    switch (e.oper_get())
      {
      case ast::OpExp::Oper::add:
        value_ = builder_.CreateAdd(left_val, right_val, "addtmp");
        break;
      case ast::OpExp::Oper::sub:
        value_ = builder_.CreateSub(left_val, right_val, "subtmp");
        break;
      case ast::OpExp::Oper::mul:
        value_ = builder_.CreateMul(left_val, right_val, "multmp");
        break;
      case ast::OpExp::Oper::div:
        value_ = builder_.CreateSDiv(left_val, right_val, "divtmp");
        break;
      case ast::OpExp::Oper::eq:
        value_ = builder_.CreateICmpEQ(left_val, right_val, "eqtmp");
        break;
      case ast::OpExp::Oper::ne:
        value_ = builder_.CreateICmpNE(left_val, right_val, "netmp");
        break;
      case ast::OpExp::Oper::lt:
        value_ = builder_.CreateICmpSLT(left_val, right_val, "lttmp");
        break;
      case ast::OpExp::Oper::le:
        value_ = builder_.CreateICmpSLE(left_val, right_val, "letmp");
        break;
      case ast::OpExp::Oper::gt:
        value_ = builder_.CreateICmpSGT(left_val, right_val, "gttmp");
        break;
      case ast::OpExp::Oper::ge:
        value_ = builder_.CreateICmpSGE(left_val, right_val, "getmp");
        break;
      default:
        unreachable();
      }

    // The comparison instructions returns an i1, and we need an i32, since everything
    // is an i32 in Tiger. Use a zero-extension to avoid this.
    value_ = builder_.CreateZExtOrTrunc(value_, i32_t(ctx_), "op_zext");
  }

  void Translator::operator()(const ast::SeqExp& e)
  {
    // An empty SeqExp is an empty expression, so we should return an int
    // containing 0, since its type is void.
    // FIXED: Some code was deleted here.
    if (e.exps_get().empty())
      {
        value_ = llvm::ConstantInt::get(i32_t(ctx_), 0);
        return;
      }

    for (const auto& exp : e.exps_get())
      {
        value_ = translate(*exp);
      }
  }

  void Translator::operator()(const ast::AssignExp& e)
  {
    // FIXED: Some code was deleted here.
    llvm::Value* ptr = access_var(e.var_get());
    llvm::Value* rhs = translate(e.exp_get());
    builder_.CreateStore(rhs, ptr);
    value_ = llvm::ConstantInt::get(i32_t(ctx_), 0);
  }

  void Translator::operator()(const ast::IfExp& e)
  {
    // FIXED: Some code was deleted here (IfExps are handled in a similar way to Kaleidoscope (see LangImpl5.html)).
    auto cond_val = translate(e.test_get());
    auto zero_val = llvm::ConstantInt::getSigned(cond_val->getType(), 0);
    auto cond_bool = builder_.CreateICmpNE(cond_val, zero_val, "ifcond");

    auto then_bb = llvm::BasicBlock::Create(ctx_, "then", current_function_);
    auto else_bb = llvm::BasicBlock::Create(ctx_, "else", current_function_);
    auto merge_bb = llvm::BasicBlock::Create(ctx_, "ifcont", current_function_);
    builder_.CreateCondBr(cond_bool, then_bb, else_bb);

    builder_.SetInsertPoint(then_bb);
    llvm::Value* then_val = translate(e.thenclause_get());
    builder_.CreateBr(merge_bb);
    then_bb = builder_.GetInsertBlock();

    builder_.SetInsertPoint(else_bb);
    bool has_else = &e.elseclause_get() != nullptr;
    llvm::Value* else_val = nullptr;
    if (has_else)
      {
        else_val = translate(e.elseclause_get());
      }
    else
      {
        llvm::Type* result_ty = llvm_type(*e.get_type());
        if (result_ty->isVoidTy())
          {
            else_val = nullptr;
          }
        else if (result_ty->isIntegerTy())
          {
            else_val = llvm::ConstantInt::get(result_ty, 0);
          }
        else
          {
            else_val = llvm::ConstantPointerNull::get(
              llvm::cast<llvm::PointerType>(result_ty));
          }
      }
    builder_.CreateBr(merge_bb);
    else_bb = builder_.GetInsertBlock();

    builder_.SetInsertPoint(merge_bb);
    llvm::Type* result_ty = llvm_type(*e.get_type());
    if (result_ty->isVoidTy())
      {
        value_ = llvm::ConstantInt::get(i32_t(ctx_), 0);
      }
    else
      {
        auto* phi = builder_.CreatePHI(result_ty, 2, "iftmp");
        phi->addIncoming(then_val, then_bb);
        phi->addIncoming(else_val, else_bb);
        value_ = phi;
      }
  }

  void Translator::operator()(const ast::WhileExp& e)
  {
    // Bb containing the test and the branching
    auto test_bb = llvm::BasicBlock::Create(ctx_, "test", current_function_);
    auto body_bb = llvm::BasicBlock::Create(ctx_, "body", current_function_);
    auto after_bb =
      llvm::BasicBlock::Create(ctx_, "afterloop", current_function_);

    // Save the after block for breaks
    loop_end_[&e] = after_bb;

    // Explicitly fall through from the current block
    builder_.CreateBr(test_bb);

    // Start inside the test BasicBlock
    builder_.SetInsertPoint(test_bb);

    auto cond_val = translate(e.test_get());
    auto zero_val = llvm::ConstantInt::getSigned(cond_val->getType(), 0);
    auto cmp_val = builder_.CreateICmpNE(cond_val, zero_val, "loopcond");

    // Create the branching
    builder_.CreateCondBr(cmp_val, body_bb, after_bb);

    // Translate the body inside the body BasicBlock
    builder_.SetInsertPoint(body_bb);
    // Don't store the return value, is should be void.
    translate(e.body_get());

    // Go back to the Test BasicBlock
    builder_.CreateBr(test_bb);

    // Continue after the loop BasicBlock
    builder_.SetInsertPoint(after_bb);
  }

  void Translator::operator()(const ast::BreakExp& e)
  {
    // FIXED: Some code was deleted here.
    auto* loop_while = dynamic_cast<const ast::WhileExp*>(e.def_get());

    // Cherche la basic block de sortie correspondant
    auto it = loop_end_.find(loop_while);

    llvm::BasicBlock* after_bb = it->second;

    builder_.CreateBr(after_bb);

    auto cont_bb =
      llvm::BasicBlock::Create(ctx_, "breakcont", current_function_);
    builder_.SetInsertPoint(cont_bb);

    value_ = llvm::ConstantInt::get(i32_t(ctx_), 0);
  }

  void Translator::operator()(const ast::ArrayExp& e)
  {
    // Translate the number of elements,
    // fill the array with the default value, then
    // return the pointer to the allocated zone.
    // FIXED: Some code was deleted here (Use `init_array`).
    auto count_val = translate(e.size_get());
    auto init_val = translate(e.init_get());
    value_ = init_array(count_val, init_val);
  }

  void Translator::operator()(const ast::CastExp& e)
  {
    auto exp_val = translate(e.exp_get());
    llvm::Type* ltype = nullptr;
    // FIXED: Some code was deleted here (Destination llvm type).
    ltype = llvm_type(*e.get_type());
    value_ = builder_.CreateBitCast(exp_val, ltype, "cast_exp");
  }

  void Translator::operator()(const ast::FunctionChunk& e)
  {
    for (const auto& fdec : e)
      visit_function_dec_header(*fdec);

    for (const auto& fdec : e)
      // There is nothing to translate for primitives.
      if (fdec->body_get())
        visit_function_dec_body(*fdec);
  }

  void Translator::visit_function_dec_header(const ast::FunctionDec& e)
  {
    bool is_main = e.name_get() == "_main";
    bool is_primitive = e.body_get() == nullptr;
    auto name = function_dec_name(e);

    const type::Type* node_type = nullptr;
    // FIXED: Some code was deleted here.
    node_type = e.get_type();
    auto& function_type = static_cast<const type::Function&>(*node_type);
    auto function_ltype = llvm_function_type(function_type);

    // Main and primitives have External linkage.
    // Other Tiger functions are treated as "static" functions in C.
    auto linkage = is_main || is_primitive ? llvm::Function::ExternalLinkage
                                           : llvm::Function::InternalLinkage;

    auto the_function =
      llvm::Function::Create(function_ltype, linkage, name, &module_);
    set_default_attributes(*the_function, e);

    // Name each argument of the function
    if (!is_main)
      {
        auto& escaped = escaped_[&function_type];
        for (auto arg_it = the_function->arg_begin();
             arg_it != the_function->arg_end(); ++arg_it)
          {
            auto i = std::distance(the_function->arg_begin(), arg_it);
            auto var = escaped.size() && static_cast<size_t>(i) < escaped.size()
              ? *std::next(escaped_[&function_type].begin(), i)
              : e.formals_get()[i - escaped.size()];

            arg_it->setName(var->name_get().get());
          }
      }
  }

  void Translator::visit_function_dec_body(const ast::FunctionDec& e)
  {
    auto the_function = module_.getFunction(function_dec_name(e));

    // Save the old function in case a nested function occurs.
    auto old_insert_point = builder_.saveIP();
    auto old_function = current_function_;
    current_function_ = the_function;

    // Create a new basic block to start the function.
    auto bb = llvm::BasicBlock::Create(ctx_, "entry_"s + e.name_get().get(),
                                       the_function);
    builder_.SetInsertPoint(bb);

    const type::Type* node_type = nullptr;
    // FIXED: Some code was deleted here.
    node_type = e.get_type();
    auto& function_type = static_cast<const type::Function&>(*node_type);
    auto& escaped = escaped_[&function_type];
    auto& formals = e.formals_get();

    auto arg_it = the_function->arg_begin();

    for (const auto var : escaped)
      {
        locals_[current_function_][var] = &*arg_it;
        ++arg_it;
      }

    // FIXED: Some code was deleted here (Create alloca instructions for each variable).
    for (const auto var : formals)
      {
        llvm::AllocaInst* local_alloca = create_alloca(
          the_function, llvm_type(*var->get_type()), var->name_get().get());
        locals_[current_function_][var] = local_alloca;
        builder_.CreateStore(&*arg_it, local_alloca);
        ++arg_it;
      }

    // FIXED: Some code was deleted here (Create a return instruction).
    llvm::Value* ret_val = translate(*e.body_get());
    if (the_function->getReturnType()->isVoidTy())
      builder_.CreateRetVoid();
    else
      builder_.CreateRet(ret_val);

    // Validate the generated code, checking for consistency.
    llvm::verifyFunction(*the_function);

    // Restore the context of the old function.
    current_function_ = old_function;
    builder_.restoreIP(old_insert_point);
  }

  void Translator::operator()(const ast::CallExp& e)
  {
    // Look up the name in the global module table.
    // If it's a primitive, rename the call to tc_name.
    //
    // Then, add the escaped variables and the rest of the arguments to the
    // list of arguments, and return the correct value.
    // FIXED: Some code was deleted here.
    std::string fname = e.name_get().get();
    if (fname == "_main")
      {
        fname = "tc_main";
      }
    else if (!module_.getFunction(fname))
      {
        fname = "tc_" + e.name_get().get();
      }

    auto callee = module_.getFunction(fname);
    std::vector<llvm::Value*> args;

    const ast::FunctionDec* fdec = e.def_get();
    const auto& ftype = static_cast<const type::Function&>(*fdec->get_type());

    auto esc_it = escaped_.find(&ftype);
    if (esc_it != escaped_.end())
      {
        for (const ast::VarDec* vd : esc_it->second)
          {
            args.push_back(locals_[current_function_][vd]);
          }
      }

    for (auto& actual : e.args_get())
      {
        args.push_back(translate(*actual));
      }

    if (callee->getReturnType()->isVoidTy())
      {
        builder_.CreateCall(callee, args);
        value_ = llvm::ConstantInt::get(i32_t(ctx_), 0);
      }
    else
      {
        value_ = builder_.CreateCall(callee, args, "calltmp");
      }
  }

  void Translator::operator()(const ast::VarDec& e)
  {
    // Void var types are actually Ints represented by a 0
    // FIXED: Some code was deleted here.
    if (current_function_->getName() == "tc_main")
      {
        llvm::Type* var_ty = llvm_type(*e.get_type());
        auto* gv = new llvm::GlobalVariable(
          module_, var_ty, false, llvm::GlobalValue::InternalLinkage,
          llvm::Constant::getNullValue(var_ty), e.name_get().get());
        llvm::Value* init_val = translate(*e.init_get());
        builder_.CreateStore(init_val, gv);
        locals_[current_function_][&e] = gv;
        value_ = llvm::ConstantInt::get(i32_t(ctx_), 0);
      }
    else
      {
        llvm::AllocaInst* local_alloca = create_alloca(
          current_function_, llvm_type(*e.get_type()), e.name_get().get());
        locals_[current_function_][&e] = local_alloca;
        llvm::Value* init_val = translate(*e.init_get());
        builder_.CreateStore(init_val, local_alloca);
        value_ = llvm::ConstantInt::get(i32_t(ctx_), 0);
      }
  }

} // namespace llvmtranslate
