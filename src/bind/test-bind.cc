// We really want to run the tests.
#undef NDEBUG
#include <cassert>
#include <iostream>

#include <ast/chunk-list.hh>
#include <ast/libast.hh>
#include <bind/libbind.hh>
#include <parse/libparse.hh>

const char* program_name = "test-parse";

// Return true on correct binding.
static bool bound(const std::string& s)
{
  ast::ChunkList* d = parse::parse_unit(s);
  misc::error e;
  // FIXME: Some code was deleted here (Run the binder).
  std::cout << *d << '\n';
  if (e)
    std::cerr << "error-->" << e << '\n';
  delete d;
  return !e;
}

int main()
{
  ast::bindings_display(std::cout) = true;

  //assert(!bound("a"));
  assert(bound("let var a := 0 in a end"));
  assert(bound("let var a := 0 in let var a := a in a end; a end"));
  assert(bound("let function f(a : int, b : string) : int = a in "
               " 5 "
               "end"));
  //assert(!bound("let function f(a : int, a : string) : int = a in "
  //             " 5 "
  //            "end"));
}
