/**
 ** \file type/array.cc
 ** \brief Implementation for type/array.hh.
 */

#include <type/array.hh>
#include <type/visitor.hh>

namespace type
{
  // FIXED
  Array::Array(const Type& type)
    : type_(type)
  {}

  bool Array::compatible_with(const Type& other) const
  {
    return this == &other.actual();
  }
  void Array::accept(ConstVisitor& v) const { v(*this); }
  void Array::accept(Visitor& v) { v(*this); }

} // namespace type
