/**
 ** \file type/type.hxx
 ** \brief Inline methods for type::Type.
 */
#pragma once

#include <misc/contract.hh>
#include <type/type.hh>

namespace type
{
  inline bool operator==(const Type& lhs, const Type& rhs)
  {
    // FIXED
    return &lhs.actual() == &rhs.actual();
  }

  inline bool operator!=(const Type& lhs, const Type& rhs)
  {
    // FIXED
    return !(lhs == rhs);
  }

} // namespace type
