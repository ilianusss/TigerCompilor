/// Test the binding with object extensions.

#include <cstdlib>
#include <iostream>

#include <ast/chunk-list.hh>
#include <ast/exp.hh>
#include <ast/libast.hh>
#include <object/libobject.hh>
#include <parse/libparse.hh>

const char* program_name = "test-bind";

// Return true on correct binding.
/*
inline static bool bound(const std::string& s)
{
  ast::ChunkList* d = parse::parse_unit(s, true);
  misc::error e;
  // FIXME: Some code was deleted here (Run the object binder).
  std::cout << *d << '\n';
  if (e)
    std::cerr << "error-->" << e << '\n';
  delete d;
  return !e;
}
*/

int main()
{
  /*
  ast::bindings_display(std::cout) = true;
  assertion(bound(" let"
                  "   class C"
                  "   {"
                  "     var a := 0"
                  "   }"
                  "   var c := new C"
                  " in"
                  "   c.a := 2"
                  " end"));
  assertion(!bound(" let"
                   "   class C"
                   "   {"
                   "     var a := 0"
                   "   }"
                   "   class D extends C"
                   "   {"
                   "     method print() : int ="
                   "       a"
                   "   }"
                   "   var d := new D"
                   " in"
                   "   d.a := 2;"
                   "   d.print()"
                   " end"));
  assertion(bound(" let"
                  "   class C"
                  "   {"
                  "     var a := 0"
                  "   }"
                  "   class D extends C"
                  "   {"
                  "     method inc_a() ="
                  "       self.a := self.a + 1"
                  "     method add_a(x : int) : int ="
                  "       if x > 0 then"
                  "       ("
                  "         self.inc_a();"
                  "         self.add_a(x - 1)"
                  "       )"
                  "   }"
                  "   var d : D := new D"
                  " in"
                  "   d.a := 2;"
                  "   d.print()"
                  " end"));
                  */
}
