/**
 ** \file ast/pretty-printer.cc
 ** \brief Implementation of ast::PrettyPrinter.
 */

#include <cstddef>
#include <istream>
#include <strstream>
#include <ast/all.hh>
#include <ast/libast.hh>
#include <ast/pretty-printer.hh>
#include <misc/escape.hh>
#include <misc/indent.hh>
#include <misc/separator.hh>

#include <type/class.hh>

namespace ast
{
  // Anonymous namespace: these functions are private to this file.
  namespace
  {
    bool in_varchunk = false;
    bool in_funcvar = false;
    bool in_formal = false;

    /// Output \a e on \a ostr.
    inline std::ostream& operator<<(std::ostream& ostr, const Escapable& e)
    {
      if (escapes_display(ostr) && e.is_escapable_get())
        ostr << "/* escaping */ ";

      return ostr;
    }

    /// \brief Output \a e on \a ostr.
    ///
    /// Used to factor the output of the name declared,
    /// and its possible additional attributes.
    inline std::ostream& operator<<(std::ostream& ostr, const Dec& e)
    {
      ostr << e.name_get();
      if (bindings_display(ostr))
        ostr << " /* " << &e << " */";
      return ostr;
    }
  } // namespace

  PrettyPrinter::PrettyPrinter(std::ostream& ostr)
    : ostr_(ostr)
  {}

  // FIXED: Some code was deleted here.

  void PrettyPrinter::operator()(const Ast& e) { super_type::operator()(e); }

  void PrettyPrinter::operator()(const SimpleVar& e)
  {
    ostr_ << e.name_get();
    if (bindings_display(ostr_))
      ostr_ << " /* " << e.def_get() << " */";
  }

  void PrettyPrinter::operator()(const FieldVar& e)
  {
    e.var_get().accept(*this);
    ostr_ << "." << e.name_get();
  }

  /* Foo[10]. */
  void PrettyPrinter::operator()(const SubscriptVar& e)
  {
    e.var_get().accept(*this);
    ostr_ << '[' << misc::incindent << e.index_get() << misc::decindent << ']';
  }

  void PrettyPrinter::operator()(const NilExp&) { ostr_ << "nil"; }

  void PrettyPrinter::operator()(const IntExp& e) { ostr_ << e.value_get(); }

  void PrettyPrinter::operator()(const StringExp& e)
  {
    ostr_ << '"' << e.value_get() << '"';
  }

  void PrettyPrinter::operator()(const CallExp& e)
  {
    ostr_ << e.name_get();
    if (bindings_display(ostr_))
      ostr_ << " /* " << e.def_get() << " */";

    ostr_ << '(';
    const auto& args = e.args_get();
    for (auto it = args.begin(); it != args.end(); ++it)
      {
        (*it)->accept(*this);
        if (std::next(it) != args.end())
          ostr_ << ", ";
      }
    ostr_ << ")";
  }

  void PrettyPrinter::operator()(const OpExp& e)
  {
    e.left_get().accept(*this);
    ostr_ << ' ' << str(e.oper_get()) << ' ';

    e.right_get().accept(*this);
  }

  void PrettyPrinter::operator()(const RecordExp& e)
  {
    ostr_ << e.type_name_get() << '{';
    const auto& args = e.fields_get();
    for (auto it = args.begin(); it != args.end(); ++it)
      {
        (*it)->accept(*this);
        if (std::next(it) != args.end())
          ostr_ << ", ";
      }
    ostr_ << '}';
  }

  void PrettyPrinter::operator()(const SeqExp& e)
  {
    const auto& exps = e.exps_get();
    int n = exps.size();

    ostr_ << '(' << misc::incindent;

    if (exps.begin() != exps.end())
      ostr_ << misc::iendl;
    int a = 0;
    for (auto it = exps.begin(); it != exps.end(); ++it)
      {
        (*it)->accept(*this);
        if (a != n - 1)
          ostr_ << ";" << misc::iendl;
        a++;
      }
    ostr_ << misc::decindent;
    if (exps.begin() != exps.end())
      ostr_ << misc::iendl;
    ostr_ << ")";
  }

  void PrettyPrinter::operator()(const AssignExp& e)
  {
    ostr_ << '(';
    e.var_get().accept(*this);
    ostr_ << " := ";
    e.exp_get().accept(*this);
    ostr_ << ')';
  }

  void PrettyPrinter::operator()(const IfExp& e)
  {
    ostr_ << "if ";
    e.test_get().accept(*this);
    ostr_ << misc::incendl << "then ";
    e.thenclause_get().accept(*this);
    if (&e.elseclause_get() != nullptr)
      {
        ostr_ << misc::iendl << "else ";
        e.elseclause_get().accept(*this);
      }
    ostr_ << misc::decindent;
  }

  void PrettyPrinter::operator()(const WhileExp& e)
  {
    ostr_ << "while";
    if (bindings_display(ostr_))
      ostr_ << " /* " << &e << " */";
    ostr_ << " ";

    e.test_get().accept(*this);
    ostr_ << misc::incendl << "do ";
    e.body_get().accept(*this);
    ostr_ << misc::decendl;
  }

  void PrettyPrinter::operator()(const ForExp& e)
  {
    ostr_ << "for";
    if (bindings_display(ostr_))
      ostr_ << " /* " << &e << " */";
    ostr_ << " ";

    e.vardec_get().accept(*this);

    ostr_ << " to ";
    e.hi_get().accept(*this);
    ostr_ << misc::incendl << "do ";
    e.body_get().accept(*this);
    ostr_ << misc::decendl;
  }

  void PrettyPrinter::operator()(const BreakExp& e)
  {
    ostr_ << "break";
    if (bindings_display(ostr_))
      ostr_ << " /* " << e.def_get() << " */";
  }

  void PrettyPrinter::operator()(const LetExp& e)
  {
    ostr_ << "let" << misc::incendl;
    e.chunks_get().accept(*this);
    ostr_ << misc::decindent << "in " << misc::incendl;
    e.body_get().accept(*this);
    ostr_ << misc::decendl << "end";
  }

  void PrettyPrinter::operator()(const ArrayExp& e)
  {
    ostr_ << e.type_name_get() << '[';
    e.size_get().accept(*this);
    ostr_ << ']' << " of ";
    e.init_get().accept(*this);
  }

  void PrettyPrinter::operator()(const CastExp& e)
  {
    ostr_ << "_cast(";
    e.exp_get().accept(*this);
    ostr_ << ", " << e.ty_get() << ')';
  }

  void PrettyPrinter::operator()(const FieldInit& e)
  {
    ostr_ << e.name_get() << " = ";
    e.init_get().accept(*this);
  }

  void PrettyPrinter::operator()(const ChunkList& e)
  {
    const auto& chunks = e.chunks_get();
    for (auto it = chunks.begin(); it != chunks.end(); ++it)
      {
        (*it)->accept(*this);
        ostr_ << misc::iendl;
      }
  }

  void PrettyPrinter::operator()(const VarChunk& e)
  {
    if (in_funcvar)
      {
        in_varchunk = true;

        for (auto it = e.begin(); it != e.end(); ++it)
          {
            (*it)->accept(*this);
            if (std::next(it) != e.end())
              ostr_ << ", ";
          }
        in_varchunk = false;
      }
    else
      {
        in_varchunk = true;

        for (auto it = e.begin(); it != e.end(); ++it)
          {
            (*it)->accept(*this);
            if (std::next(it) != e.end())
              ostr_ << misc::iendl;
          }
        in_varchunk = false;
      }
  }

  void PrettyPrinter::operator()(const VarDec& e)
  {
    if (in_varchunk && !in_funcvar)
      ostr_ << "var ";

    ostr_ << static_cast<const Escapable&>(e);

    ostr_ << static_cast<const Dec&>(e);

    if (e.type_name_get() != nullptr)
      {
        ostr_ << " : ";
        e.type_name_get()->accept(*this);
      }
    if (e.init_get() != nullptr)
      {
        ostr_ << " := ";
        e.init_get()->accept(*this);
      }
  }

  void PrettyPrinter::operator()(const FunctionChunk& e)
  {
    for (auto it = e.begin(); it != e.end(); ++it)
      {
        (*it)->accept(*this);
        ostr_ << misc::iendl;
      }
  }

  void PrettyPrinter::operator()(const FunctionDec& e)
  {
    if (e.body_get() == nullptr)
      ostr_ << "primitive ";
    else
      ostr_ << "function ";

    ostr_ << static_cast<const Dec&>(e);

    ostr_ << '(';
    in_funcvar = true;
    e.formals_get().accept(*this);
    in_funcvar = false;
    ostr_ << ')';
    if (e.result_get() != nullptr)
      {
        ostr_ << " : ";
        e.result_get()->accept(*this);
      }
    if (e.body_get() != nullptr)
      {
        ostr_ << " =" << misc::incendl;
        e.body_get()->accept(*this);
        ostr_ << misc::decendl;
      }
  }

  void PrettyPrinter::operator()(const TypeChunk& e)
  {
    for (const auto dec : e)
      dec->accept(*this);
  }

  void PrettyPrinter::operator()(const TypeDec& e)
  {
    ostr_ << "type ";

    ostr_ << static_cast<const Dec&>(e);

    ostr_ << " = " << e.ty_get() << misc::iendl;
  }

  void PrettyPrinter::operator()(const NameTy& e)
  {
    ostr_ << e.name_get();

    if (bindings_display(ostr_))
      ostr_ << " /* " << e.def_get() << " */";
  }

  void PrettyPrinter::operator()(const RecordTy& e)
  {
    ostr_ << '{';
    const auto& args = e.fields_get();
    for (auto it = args.begin(); it != args.end(); ++it)
      {
        (*it)->accept(*this);
        if (std::next(it) != args.end())
          ostr_ << ", ";
      }
    ostr_ << '}';
  }

  void PrettyPrinter::operator()(const ArrayTy& e)
  {
    ostr_ << "array of " << e.base_type_get();
  }

  void PrettyPrinter::operator()(const Field& e)
  {
    ostr_ << e.name_get() << " : " << e.type_name_get();
  }

} // namespace ast
