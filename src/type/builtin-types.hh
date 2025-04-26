/**
 ** \file type/builtin-types.hh
 ** \brief The classes Int, String, Void.
 */
#pragma once

#include <misc/singleton.hh>
#include <type/fwd.hh>
#include <type/type.hh>

namespace type
{
  // FIXED
  class Int
    : public Type
    , public misc::Singleton<type::Int>
  {
  public:
    Int() = default;
    ~Int() {}
    const Type& actual() const override;

    void accept(Visitor& v) override;
    void accept(ConstVisitor& v) const override;
  };

  class String
    : public Type
    , public misc::Singleton<type::String>
  {
  public:
    String() = default;
    ~String() {}
    const Type& actual() const override;

    void accept(Visitor& v) override;
    void accept(ConstVisitor& v) const override;
  };

  class Void
    : public Type
    , public misc::Singleton<type::Void>
  {
  public:
    Void() = default;
    ~Void() {}
    const Type& actual() const override;

    void accept(Visitor& v) override;
    void accept(ConstVisitor& v) const override;
  };

} // namespace type
