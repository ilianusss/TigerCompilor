/**
 ** \file type/builtin-types.cc
 ** \brief Implementation for type/builtin-types.hh.
 */

#include <ostream>

#include <type/builtin-types.hh>
#include <type/visitor.hh>

namespace type
{
  // FIXED
  void Int::accept(Visitor& v) { v(*this); }

  void Int::accept(ConstVisitor& v) const { v(*this); }

  const Type& Int::actual() const { return Int::instance(); }

  void String::accept(Visitor& v) { v(*this); }

  void String::accept(ConstVisitor& v) const { v(*this); }

  const Type& String::actual() const { return String::instance(); }

  void Void::accept(Visitor& v) { v(*this); }

  void Void::accept(ConstVisitor& v) const { v(*this); }

  const Type& Void::actual() const { return Void::instance(); }

} // namespace type
