/**
 ** \file type/array.hh
 ** \brief The class Array.
 */
#pragma once

#include <type/fwd.hh>
#include <type/type.hh>

namespace type
{
  /// Array types.
  class Array : public Type
  {
    // FIXED
  public:
    Array(const Type& type);
    ~Array() = default;
    bool compatible_with(const Type& other) const override;
    void accept(ConstVisitor& v) const override;
    void accept(Visitor& v) override;
    const Type& type_get() const;

  private:
    const Type& type_;
  };

} // namespace type

#include <type/array.hxx>
