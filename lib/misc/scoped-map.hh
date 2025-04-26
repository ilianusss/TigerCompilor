/**
 ** \file misc/scoped-map.hh
 ** \brief Declaration of misc::scoped_map.
 **
 ** This implements a stack of dictionnaries.  Each time a scope is
 ** opened, a new dictionnary is added on the top of the stack; the
 ** dictionary is removed when the scope is closed.  Lookup of keys
 ** is done in the last added dictionnary first (LIFO).
 **
 ** In particular this class is used to implement symbol tables.
 **/

#pragma once

#include <map>
#include <ostream>
#include <vector>

namespace misc
{
  // FIXED: The concept declaration was deleted.
  template <typename Key, typename Data>
  concept mappable = std::copyable<Data>;

  template <typename Key, typename Data>
    requires mappable<Key, Data>
  class scoped_map
  {
    // FIXED: Some code was deleted here.
  public:
    scoped_map();
    ~scoped_map() = default;

    void scope_begin();
    void scope_end();
    void put(Key& key, Data& data);
    const Data& get(Key& key);
    std::ostream& dump(std::ostream& ostr) const;
    std::map<Key, Data>& get_last_scope();

  private:
    using key_type = std::remove_const_t<Key>;
    std::vector<std::map<key_type, Data>> scopes_;
  };

  template <typename Key, typename Data>
  std::ostream& operator<<(std::ostream& ostr,
                           const scoped_map<Key, Data>& tbl);

} // namespace misc

#include <misc/scoped-map.hxx>
