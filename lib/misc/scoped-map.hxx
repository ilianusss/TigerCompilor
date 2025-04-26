/** \file misc/scoped-map.hxx
 ** \brief Implementation of misc::scoped_map.
 */

#pragma once

#include <sstream>
#include <stdexcept>
#include <type_traits>

#include <ranges>
#include <misc/algorithm.hh>
#include <misc/contract.hh>
#include <misc/indent.hh>

namespace misc
{
  // FIXED: Some code was deleted here.
  //
  template <typename Key, typename Data>
    requires mappable<Key, Data>
  inline scoped_map<Key, Data>::scoped_map()
    : scopes_()
  {
    scopes_.emplace_back();
  }

  template <typename Key, typename Data>
    requires mappable<Key, Data>
  inline void scoped_map<Key, Data>::scope_begin()
  {
    scopes_.emplace_back();
  }

  template <typename Key, typename Data>
    requires mappable<Key, Data>
  inline void scoped_map<Key, Data>::scope_end()
  {
    if (scopes_.empty())
      throw std::runtime_error("No scope to pop.");
    if (scopes_.size() == 1)
      throw std::runtime_error("Cannot pop the global scope.");
    scopes_.pop_back();
  }

  template <typename Key, typename Data>
    requires mappable<Key, Data>
  inline void scoped_map<Key, Data>::put(Key& key, Data& data)
  {
    auto& current_scope = scopes_.back();
    current_scope[std::move(key)] = std::move(data);
  }

  template <typename Key, typename Data>
    requires mappable<Key, Data>
  inline const Data& scoped_map<Key, Data>::get(Key& key)
  {
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it)
      {
        auto found = it->find(key);
        if (found != it->end())
          {
            return found->second;
          }
      }
    throw std::runtime_error("Key not found in any scope.");
  }

  template <typename Key, typename Data>
    requires mappable<Key, Data>
  inline std::ostream& scoped_map<Key, Data>::dump(std::ostream& ostr) const
  {
    int level = 0;
    for (const auto& scope : scopes_)
      {
        ostr << "Scope level " << level++ << ":\n";
        for (const auto& [k, d] : scope)
          {
            ostr << "  " << k << " -> " << d << "\n";
          }
      }
    return ostr;
  }

  template <typename Key, typename Data>
    requires mappable<Key, Data>
  inline std::map<Key, Data>& scoped_map<Key, Data>::get_last_scope()
  {
    return scopes_.back();
  }

  template <typename Key, typename Data>
  inline std::ostream& operator<<(std::ostream& ostr,
                                  const scoped_map<Key, Data>& tbl)
  {
    return tbl.dump(ostr);
  }

} // namespace misc
