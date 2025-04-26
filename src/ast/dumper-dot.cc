/**
 ** \file ast/dumper-dot.cc
 ** \brief Implementation of ast::DumperDot.
 */

#include <ast/all.hh>
#include <ast/dumper-dot.hh>
#include <misc/indent.hh>
#include <misc/symbol.hh>
#include <type/pretty-printer.hh>

namespace ast
{
  using namespace ast;

  void DumperDot::dump(const std::string& field, const ast::Ast& e)
  {
    const std::string* old_parent_field = parent_field;
    parent_field = &field;
    e.accept(*this);
    parent_field = old_parent_field;
  }

  void DumperDot::dump(const std::string& field, const ast::Ast* e)
  {
    if (!e)
      return;
    const std::string* old_parent_field = parent_field;
    parent_field = &field;
    e->accept(*this);
    parent_field = old_parent_field;
  }

  DumperDot::DumperDot(std::ostream& ostr)
    : ostr_(ostr)
  {}

  void DumperDot::dump_type(const ast::Typable& e)
  {
    // FIXME: Some code was deleted here (Call node_html_field on a e.type_get() if exist).
  }

  void DumperDot::operator()(const ArrayExp& e)
  {
    unsigned long old_parent_id = node_html_header(e, "ArrayExp");
    // FIXED
    node_html_field("type", e.type_name_get().name_get());
    node_html_ports({"type_name", "size", "init"});
    dump_type(e);
    footer_and_link(old_parent_id);
    dump("type_name", e.type_name_get());
    dump("size", e.size_get());
    dump("init", e.init_get());
    parent_id = old_parent_id;
  }

  void DumperDot::operator()(const ArrayTy& e)
  {
    unsigned long old_parent_id = node_html_header(e, "ArrayTy");
    dump_type(e);
    node_html_ports({"base_type"});
    footer_and_link(old_parent_id);
    dump("base_type", e.base_type_get());
    parent_id = old_parent_id;
  }

  void DumperDot::operator()(const AssignExp& e)
  {
    unsigned long old_parent_id = node_html_header(e, "AssignExp");
    // FIXED
    node_html_ports({"var", "exp"});
    dump_type(e);
    footer_and_link(old_parent_id);
    dump("var", e.var_get());
    dump("exp", e.exp_get());
    parent_id = old_parent_id;
  }

  void DumperDot::operator()(const BreakExp& e)
  {
    unsigned long old_parent_id = node_html_header(e, "BreakExp");
    dump_type(e);
    node_html_ports({"def"});
    footer_and_link(old_parent_id);
    dump_def(e);
    parent_id = old_parent_id;
  }

  void DumperDot::operator()(const CallExp& e)
  {
    unsigned long old_parent_id = node_html_header(e, "CallExp");
    // FIXED
    node_html_field("name", e.name_get());
    dump_type(e);
    node_html_ports();
    node_html_port_list("args", e.args_get());
    node_html_one_port("def");
    footer_and_link(old_parent_id);
    dump_def(e);
    dump_list("args", e.args_get());
    parent_id = old_parent_id;
  }

  void DumperDot::operator()(const CastExp& e)
  {
    unsigned long old_parent_id = node_html_header(e, "CastExp");
    dump_type(e);
    node_html_ports({"exp", "ty"});
    footer_and_link(old_parent_id);
    dump("exp", e.exp_get());
    dump("ty", e.ty_get());
    parent_id = old_parent_id;
  }

  void DumperDot::operator()(const ClassTy& e)
  {
    unsigned long old_parent_id = node_html_header(e, "ClassTy");
    dump_type(e);
    node_html_ports({"super", "chunks"});
    footer_and_link(old_parent_id);
    dump("super", e.super_get());
    dump("chunks", e.chunks_get());
    parent_id = old_parent_id;
  }

  void DumperDot::operator()(const Field& e)
  {
    unsigned long old_parent_id = node_html_header(e, "Field");
    node_html_field("name", e.name_get());
    node_html_ports({"type_name"});
    footer_and_link(old_parent_id);
    dump("type_name", e.type_name_get());
    parent_id = old_parent_id;
  }

  void DumperDot::operator()(const FieldInit& e)
  {
    unsigned long old_parent_id = node_html_header(e, "FieldInit");
    node_html_field("name", e.name_get());
    node_html_ports({"init"});
    footer_and_link(old_parent_id);
    dump("init", e.init_get());
    parent_id = old_parent_id;
  }

  void DumperDot::operator()(const FieldVar& e)
  {
    unsigned long old_parent_id = node_html_header(e, "FieldVar");
    // FIXED
    node_html_field("field", e.name_get());
    node_html_ports({"var"});
    dump_type(e);
    footer_and_link(old_parent_id);
    dump("var", e.var_get());
    parent_id = old_parent_id;
  }

  void DumperDot::operator()(const ForExp& e)
  {
    unsigned long old_parent_id = node_html_header(e, "ForExp");
    dump_type(e);
    node_html_ports({"vardec", "hi", "body"});
    footer_and_link(old_parent_id);
    dump("vardec", e.vardec_get());
    dump("hi", e.hi_get());
    dump("body", e.body_get());
    parent_id = old_parent_id;
  }

  void DumperDot::operator()(const FunctionDec& e)
  {
    unsigned long old_parent_id = node_html_header(e, "FunctionDec");
    dump_type(e);
    node_html_field("name", e.name_get());
    node_html_ports({"formals", "result", "body"});
    footer_and_link(old_parent_id);
    dump("formals", e.formals_get());
    dump("result", e.result_get());
    dump("body", e.body_get());
    parent_id = old_parent_id;
  }

  void DumperDot::operator()(const IfExp& e)
  {
    unsigned long old_parent_id = node_html_header(e, "IfExp");
    // FIXED
    if (&e.elseclause_get() != nullptr)
      node_html_ports({"test", "thenclause", "elseclause"});
    else
      node_html_ports({"test", "thenclause"});
    dump_type(e);
    footer_and_link(old_parent_id);
    dump("test", e.test_get());
    dump("thenclause", e.thenclause_get());
    if (&e.elseclause_get() != nullptr)
      dump("elseclause", e.elseclause_get());
    parent_id = old_parent_id;
  }

  void DumperDot::operator()(const IntExp& e)
  {
    unsigned long old_parent_id = node_html_header(e, "IntExp");
    dump_type(e);
    node_html_field("value", e.value_get());
    footer_and_link(old_parent_id);
    parent_id = old_parent_id;
  }

  void DumperDot::operator()(const LetExp& e)
  {
    unsigned long old_parent_id = node_html_header(e, "LetExp");
    // FIXED
    node_html_ports({"chunks", "body"});
    dump_type(e);
    footer_and_link(old_parent_id);
    dump("chunks", e.chunks_get());
    dump("body", e.body_get());
    parent_id = old_parent_id;
  }

  void DumperDot::operator()(const MethodCallExp& e)
  {
    unsigned long old_parent_id = node_html_header(e, "MethodCallExp");
    // FIXED
    node_html_field("name", e.name_get());
    node_html_ports({"object", "args"});
    node_html_port_list("args", e.args_get());
    dump_type(e);
    node_html_one_port("def");
    footer_and_link(old_parent_id);
    dump_def(e);
    dump("object", e.object_get());
    dump_list("args", e.args_get());
    parent_id = old_parent_id;
  }

  void DumperDot::operator()(const MethodDec& e)
  {
    unsigned long old_parent_id = node_html_header(e, "MethodDec");
    dump_type(e);
    node_html_field("name", e.name_get());
    node_html_ports({"formals", "result", "body"});
    footer_and_link(old_parent_id);
    dump("formals", e.formals_get());
    dump("result", e.result_get());
    dump("body", e.body_get());
    parent_id = old_parent_id;
  }

  void DumperDot::operator()(const NameTy& e)
  {
    unsigned long old_parent_id = node_html_header(e, "NameTy");
    dump_type(e);
    node_html_field("name", e.name_get());
    node_html_ports({"def"});
    footer_and_link(old_parent_id);
    dump_def(e);
    parent_id = old_parent_id;
  }

  void DumperDot::operator()(const NilExp& e)
  {
    unsigned long old_parent_id = node_html_header(e, "NilExp");
    dump_type(e);
    footer_and_link(old_parent_id);
    parent_id = old_parent_id;
  }

  void DumperDot::operator()(const ObjectExp& e)
  {
    unsigned long old_parent_id = node_html_header(e, "ObjectExp");
    // FIXED
    node_html_ports({"type_name"});
    dump_type(e);
    footer_and_link(old_parent_id);
    dump("type_name", e.type_name_get());
    parent_id = old_parent_id;
  }

  void DumperDot::operator()(const OpExp& e)
  {
    unsigned long old_parent_id = node_html_header(e, "OpExp");
    dump_type(e);
    node_html_field("oper", str(e.oper_get()), "'");
    node_html_ports({"left", "right"});
    footer_and_link(old_parent_id);
    dump("left", e.left_get());
    dump("right", e.right_get());
    parent_id = old_parent_id;
  }

  void DumperDot::operator()(const RecordExp& e)
  {
    unsigned long old_parent_id = node_html_header(e, "RecordExp");
    // FIXED
    node_html_ports({"type_name"});
    node_html_port_list("fields", e.fields_get());
    dump_type(e);
    footer_and_link(old_parent_id);
    dump("type_name", e.type_name_get());
    dump_list("fields", e.fields_get());
    parent_id = old_parent_id;
  }

  void DumperDot::operator()(const RecordTy& e)
  {
    unsigned long old_parent_id = node_html_header(e, "RecordTy");
    dump_type(e);
    node_html_ports();
    // FIXED
    node_html_port_list("fields", e.fields_get());
    footer_and_link(old_parent_id);
    dump_list("fields", e.fields_get());
    parent_id = old_parent_id;
  }

  void DumperDot::operator()(const SeqExp& e)
  {
    unsigned long old_parent_id = node_html_header(e, "SeqExp");
    dump_type(e);
    node_html_ports();
    // FIXED
    node_html_port_list("exps", e.exps_get());
    footer_and_link(old_parent_id);
    dump_list("exps", e.exps_get());
    parent_id = old_parent_id;
  }

  void DumperDot::operator()(const SimpleVar& e)
  {
    unsigned long old_parent_id = node_html_header(e, "SimpleVar");
    dump_type(e);
    node_html_field("name", e.name_get());
    node_html_ports({"def"});
    footer_and_link(old_parent_id);
    dump_def(e);
    parent_id = old_parent_id;
  }

  void DumperDot::operator()(const StringExp& e)
  {
    unsigned long old_parent_id = node_html_header(e, "StringExp");
    // FIXED
    node_html_field("string", misc::escape(e.value_get()));
    dump_type(e);
    footer_and_link(old_parent_id);
    parent_id = old_parent_id;
  }

  void DumperDot::operator()(const SubscriptVar& e)
  {
    unsigned long old_parent_id = node_html_header(e, "SubscriptVar");
    dump_type(e);
    node_html_ports({"var", "index"});
    footer_and_link(old_parent_id);
    dump("var", e.var_get());
    dump("index", e.index_get());
    parent_id = old_parent_id;
  }

  void DumperDot::operator()(const TypeDec& e)
  {
    unsigned long old_parent_id = node_html_header(e, "TypeDec");
    dump_type(e);
    node_html_field("name", e.name_get());
    node_html_ports({"ty"});
    footer_and_link(old_parent_id);
    dump("ty", e.ty_get());
    parent_id = old_parent_id;
  }

  void DumperDot::operator()(const VarDec& e)
  {
    unsigned long old_parent_id = node_html_header(e, "VarDec");
    dump_type(e);
    node_html_field("name", e.name_get());
    node_html_ports({"type_name", "init"});
    node_html_escapable_field(e);
    footer_and_link(old_parent_id);
    dump("type_name", e.type_name_get());
    dump("init", e.init_get());
    parent_id = old_parent_id;
  }

  void DumperDot::operator()(const WhileExp& e)
  {
    unsigned long old_parent_id = node_html_header(e, "WhileExp");
    dump_type(e);
    node_html_ports({"test", "body"});
    footer_and_link(old_parent_id);
    dump("test", e.test_get());
    dump("body", e.body_get());
    parent_id = old_parent_id;
  }

  void DumperDot::operator()(const ast::ChunkList& e)
  {
    dump_chunk<ast::ChunkList>(e, "ChunkList");
  }

  void DumperDot::operator()(const ast::FunctionChunk& e)
  {
    dump_chunk<ast::FunctionChunk>(e, "FunctionChunk");
  }

  void DumperDot::operator()(const ast::MethodChunk& e)
  {
    dump_chunk<ast::MethodChunk>(e, "MethodChunk");
  }

  void DumperDot::operator()(const ast::TypeChunk& e)
  {
    dump_chunk<ast::TypeChunk>(e, "TypeChunk");
  }

  void DumperDot::operator()(const ast::VarChunk& e)
  {
    dump_chunk<ast::VarChunk>(e, "VarChunk");
  }
} // namespace ast
