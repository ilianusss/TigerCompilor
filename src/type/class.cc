/**
 ** \file type/class.cc
 ** \brief Implementation for type/class.hh.
 */

#include <ostream>

#include <misc/algorithm.hh>
#include <type/class.hh>
#include <type/visitor.hh>

namespace type
{
  Class::Class(const Class* super)
    : Type()
    , id_(fresh_id())
    , super_(super)
    , subclasses_()
  {}

  void Class::accept(ConstVisitor& v) const { v(*this); }

  void Class::accept(Visitor& v) { v(*this); }

  const Type* Class::attr_type(misc::symbol key) const
  {
    // FIXED
    for (auto& a : attrs_)
      {
        if (a.name_get() == key)
          return &a.type_get();
      }
    return nullptr;
  }

  const Type* Class::meth_type(misc::symbol key) const
  {
    // FIXED
    for (auto m : meths_)
      {
        if (m->name_get() == key)
          return &m->type_get();
      }
    return nullptr;
  }

  // FIXED
  const Class* Class::common_root(const Class& other) const
  {
    std::vector<const Class*> her;
    const Class* cur = super_;
    while (*cur != object_instance())
      {
        her.push_back(cur);
        cur = cur->super_;
      }
    cur = other.super_;
    while (*cur != object_instance())
      {
        if (std::find(her.begin(), her.end(), cur) != her.end())
          return cur;
        cur = cur->super_;
      }
    return &object_instance();
  }

  // FIXED
  bool Class::sound() const
  {
    const Class* curr = super_;
    while (curr != &object_instance())
      {
        curr = curr->super_;
      }
    return true;
  }

  // FIXED
  bool Class::compatible_with(const Type& other) const
  {
    if (const Class* oth = dynamic_cast<const Class*>(&other.actual()))
      return im_parent(*this, *oth);

    return dynamic_cast<const Nil*>(&other.actual()) || this == &other.actual();
  }

  bool Class::im_parent(const Class& me, const Class& other) const
  {
    const Class* curr = &other;
    while (curr && *curr != object_instance())
      {
        if (&me == curr)
          return true;
        curr = curr->super_get();
      }
    return false;
  }

  const Class& Class::object_instance()
  {
    // FIXED
    return misc::Singleton<type::Class>::instance();
  }

  unsigned Class::fresh_id()
  {
    static unsigned counter_ = 0;
    return counter_++;
  }

} // namespace type
