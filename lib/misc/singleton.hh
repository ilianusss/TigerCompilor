/**
 ** \file misc/singleton.hh
 ** \brief Generic singleton
 */

#pragma once

namespace misc
{
  template <typename T> class Singleton
  {
    // FIXED
  public:
    static T& instance()
    {
      static T instance;
      return instance;
    }

  protected:
    Singleton() = default;
    ~Singleton() = default;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
  };

} // namespace misc
