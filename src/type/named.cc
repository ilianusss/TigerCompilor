/**
 ** \file type/named.cc
 ** \brief Implementation for type/named.hh.
 */

#include <algorithm>
#include <list>
#include <set>

#include <type/named.hh>
#include <type/visitor.hh>

namespace type
{
  Named::Named(misc::symbol name)
    : name_(name)
    , type_(nullptr)
  {}

  Named::Named(misc::symbol name, const Type* type)
    : name_(name)
    , type_(type)
  {}

  // Inherited functions
  void Named::accept(ConstVisitor& v) const
  {
    // FIXED
    v(*this);
  }

  void Named::accept(Visitor& v)
  {
    // FIXED
    v(*this);
  }

  bool Named::sound() const
  {
    // FIXED
    std::list<const Type*> visited;
    for (auto t = this; t; t = dynamic_cast<const Named*>(t->type_))
      {
        const Type* as_type = static_cast<const Type*>(t);
        if (std::find(visited.begin(), visited.end(), as_type) != visited.end())
          return false;
        visited.push_back(as_type);
      }
    return true;
  }

  bool Named::compatible_with(const Type& other) const
  {
    // FIXED
    return type_ ? type_->compatible_with(other) : false;
  }

} // namespace type
