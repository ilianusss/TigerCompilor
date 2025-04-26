/**
 ** \file type/pretty-printer.cc
 ** \brief Implementation for type/pretty-printer.hh.
 */

#include <type/libtype.hh>
#include <type/pretty-printer.hh>
#include <type/type.hh>
#include <type/types.hh>

namespace type
{
  namespace
  {
    template <typename Type>
    std::ostream& print_type(std::ostream& ostr, const Type& type)
    {
      PrettyPrinter printer{ostr};
      printer(type);
      return ostr;
    }

    /// How many times did we go through operator()(const Named&)?
    inline long int& indent(std::ostream& o)
    {
      // The slot to store the current indentation level.
      static const int indent_index = std::ios::xalloc();
      return o.iword(indent_index);
    }

  } // namespace

  std::ostream& operator<<(std::ostream& ostr, const Attribute& e)
  {
    return print_type(ostr, e);
  }

  std::ostream& operator<<(std::ostream& ostr, const Field& e)
  {
    return print_type(ostr, e);
  }

  std::ostream& operator<<(std::ostream& ostr, const Type& e)
  {
    return print_type(ostr, e);
  }

  PrettyPrinter::PrettyPrinter(std::ostream& ostr)
    : ostr_{ostr}
  {}

  void PrettyPrinter::operator()(const Nil& e)
  {
    ostr_ << "nil = ";
    if (auto record_type = e.record_type_get())
      ostr_ << *record_type;
    else
      ostr_ << "(null)";
  }

  void PrettyPrinter::operator()(const Void&) { ostr_ << "void"; }

  void PrettyPrinter::operator()(const Int&)
  {
    // FIXED
    ostr_ << "int";
  }

  void PrettyPrinter::operator()(const String&)
  {
    // FIXED
    ostr_ << "string";
  }

  void PrettyPrinter::operator()(const Named& e)
  {
    // FIXED
    ostr_ << "named: " << e.name_get();
    if (!e.sound())
      ostr_ << ": " << e.actual();
  }

  void PrettyPrinter::operator()(const Array& e)
  {
    // FIXED
    ostr_ << "array of " << e.type_get();
  }

  void PrettyPrinter::operator()(const Record& e)
  {
    // FIXED
    ostr_ << "{";
    ostr_ << misc::incendl;
    for (const auto& field : e.fields_get())
      {
        ostr_ << field;
        if (&field != &e.fields_get().back())
          ostr_ << ", ";
      }
    ostr_ << misc::decendl;
    ostr_ << "}";
  }

  void PrettyPrinter::operator()(const Class& e)
  {
    // FIXED
    ostr_ << "class ";
    if (e.super_get())
      ostr_ << *e.super_get();
    else
      ostr_ << "Object";
    if (!e.has_data())
      return;
    ostr_ << " {";
    ostr_ << misc::incendl;
    for (const auto& att : e.attrs_get())
      {
        ostr_ << att;
        if (&att != &e.attrs_get().back())
          ostr_ << ", ";
      }
    ostr_ << misc::decendl;
    for (const auto& meth : e.meths_get())
      {
        ostr_ << meth;
        if (&meth != &e.meths_get().back())
          ostr_ << ", ";
      }
    ostr_ << "}";
  }

  void PrettyPrinter::operator()(const Function& e)
  {
    // FIXED
    ostr_ << "(" << e.formals_get() << ")"
          << " : " << e.result_get();
  }

  void PrettyPrinter::operator()(const Attribute& e)
  {
    ostr_ << e.name_get() << " : " << e.type_get();
  }

  void PrettyPrinter::operator()(const Field& e)
  {
    ostr_ << e.name_get() << " : " << e.type_get();
  }

} // namespace type
