/**
 ** \file type/record.cc
 ** \brief Implementation for type/record.hh.
 */

#include <ostream>

#include <type/builtin-types.hh>
#include <type/nil.hh>
#include <type/record.hh>
#include <type/visitor.hh>

namespace type
{
  void Record::accept(ConstVisitor& v) const { v(*this); }

  void Record::accept(Visitor& v) { v(*this); }

  // FIXED
  const Type* Record::field_type(misc::symbol key) const
  {
    for (const Field& field : fields_)
      if (field.name_get() == key)
        return &field.type_get();
    return nullptr;
  }

  int Record::field_index(misc::symbol key) const
  {
    int i = 0;
    for (const Field& field : fields_)
      {
        if (field.name_get() == key)
          return i;
        ++i;
      }
    return -1;
  }

  // FIXED
  bool Record::compatible_with(const Type& other) const
  {
    return dynamic_cast<const Nil*>(&other) || this == &other.actual();
  }

} // namespace type
