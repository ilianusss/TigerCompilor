dnl Catch macros that are not expanded.
m4_pattern_forbid([^(AM|AX|LT|TC)_])

m4_define([TC_CLANG_PREREQ],    [18.0])
m4_define([TC_GCC_PREREQ],      [10.2])
m4_define([TC_HAVM_PREREQ],     [0.28])
m4_define([TC_NOLIMIPS_PREREQ], [0.11])

AC_PREREQ([2.64])

## ---------------- ##
## Package set up.  ##
## ---------------- ##

AC_INIT([LRE Tiger Compiler], [1.94.d], [tiger@lrde.epita.fr], [tc],
        [http://tiger.lrde.epita.fr])

# Auxiliary files.
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([build-aux/m4])

# Automake.
AM_INIT_AUTOMAKE([1.14.1 dist-bzip2 no-dist-gzip foreign
                  color-tests parallel-tests
                  nostdinc silent-rules -Wall])
# By default, let "make" hide the compilation command and display a
# "GEN ..." message, unless V=1 is passed to "make".
AM_SILENT_RULES([yes])


## --------------------- ##
## C++ Compiler Set up.  ##
## --------------------- ##

# Look for a C++ compiler.
AC_LANG([C++])
AC_PROG_CXX

# Warn if the C++ compiler is not known to support the required
# features.  Or course, we should rather check for features, not
# version numbers.
case $($CXX --version) in
  (*clang*)
    AS_VERSION_COMPARE([$($CXX -dumpversion)], [TC_CLANG_PREREQ],
      [AC_MSG_ERROR([clang++ >= TC_CLANG_PREREQ is required, have $cxx_version])])
    ;;
  (*g++*)
    AS_VERSION_COMPARE([$($CXX -dumpfullversion)], [TC_GCC_PREREQ],
      [AC_MSG_ERROR([g++ >= TC_GCC_PREREQ is required, have $cxx_version])])
    ;;
  (*)
    AC_MSG_ERROR([g++ >= TC_GCC_PREREQ or clang++ >= TC_CLANG_PREREQ is required, have none])
    ;;
esac

# Enable C++ 2014 support.
AC_CANONICAL_HOST
# Passing -stdlib=libc++ to clang++ is a requirement on OS X to enable
# full C++ 2011 support.  For more information on this, see
# http://cplusplusmusings.wordpress.com/2012/07/05/clang-and-standard-libraries
# We don't try to enable it by default on other platforms, as libc++
# is principally supported by OS X.
case $host_os in
  (darwin*)
    AX_CHECK_COMPILE_FLAG([-std=c++20 -stdlib=libc++],
                          [CXXFLAGS="$CXXFLAGS -std=c++20 -stdlib=libc++"
                           stdpass=true])
    ;;
esac
if ! "${stdpass-false}"; then
  AX_CHECK_COMPILE_FLAG([-std=c++20],
                        [CXXFLAGS="$CXXFLAGS -std=c++20"
                         stdpass=true])
fi

if ! "${stdpass-false}"; then
  AC_MSG_ERROR([unable to turn on C++20 mode with this compiler])
fi

# Try to use pipes between compiler stages to speed the compilation up.
AX_CHECK_COMPILE_FLAG([-pipe], [CXXFLAGS="$CXXFLAGS -pipe"])

# Use good warnings.
TC_CXX_WARNINGS([[-Wall],
                 [-W],
                 [-Wcast-align],
                 [-Wcast-qual],
                 [-Wformat],
                 [-Wmissing-prototypes],
                 [-Wstrict-prototypes],
                 [-Wwrite-strings],
                 [-Wbad-function-cast],
                 [-Wmissing-declarations],
                 [-Wnested-externs],
                 [-Wunused-parameter],
                 [-Wunused-function],
                 [-Woverloaded-virtual],
])

# Silence bison 3.8.2 generated code warnings
#
# parse/parsetiger.cc:2956:17: warning: 'parse::parser::glr_stack' has a field
# 'parse::parser::glr_stack::yyerror_range' whose type uses the anonymous
# namespace [-Wsubobject-linkage]
#
# parse/parsetiger.cc:2956:17: warning: 'parse::parser::glr_stack' has a field
# 'parse::parser::glr_stack::yystateStack' whose type uses the anonymous
# namespace [-Wsubobject-linkage]

TC_CXX_WARNINGS([[-Wno-shadow]])

# Pacify g++ on Boost Variants.
TC_CXX_WARNINGS([[-Wno-subobject-linkage]])

# This is so that ICC no longer complain that
#
#  scantiger.ll(177): remark #111: statement is unreachable
#          break;
#          ^
#
#   ./../ast/seqexp.hh(36): remark #193:
#      zero used for undefined preprocessing identifier
#   #if STUDENT
#       ^
#
#    scantiger.cc(924): remark #279: controlling expression is constant
#          while ( 1 )             /* loops until end-of-file is reached */
#                  ^
#
#   ../../src/task/task.hh(38): remark #383:
#     value copied to temporary, reference to
#    temporary used
#           int key = 0, const std::string& deps = "");
#                                                  ^
#   /intel/compiler70/ia32/include/xstring(41): remark #444:
#      destructor for base class "std::_String_val<_Elem, _Ax>" is not virtual
#                   : public _String_val<_Elem, _Ax>
#                            ^
#
#  ../../../src/type/types.hxx(64): remark #522:
#    function "type::Named::type_set(const type::Type *)" redeclared "inline"
#    after being called
#      Named::type_set (const Type* type)
#              ^
#
#  ./../ast/print-visitor.hh(21): warning #654:
#  overloaded virtual function "ast::GenVisitor<K>::visit
#                               [with K=ast::const_kind]"
#   is only partially overridden in class "ast::PrettyPrinter"
#      class PrettyPrinter : public DefaultVisitor<const_kind>
#            ^
#
#  /intel/compiler70/ia32/include/xlocale(1381): remark #810:
#     conversion from "int" to "char" may lose significant bits
#                  return (widen((char)_Tolower((unsigned char)narrow(_Ch),
#                                ^
#
#
#   ./../ast/print-visitor.hh(331): remark #981:
#      operands are evaluated in unspecified order
#           _ostr << "type " << e.name_get () << " = ";
#                               ^
#
#  scantiger.cc(324): remark #1418:
#     external definition with no prior declaration
#    static char yy_hold_char;
#                ^
#
TC_CXX_WARNINGS([[[-wd111,193,279,383,444,522,654,810,981,1418]]])

# GCC emits tons of warnings about src/parse/parsetiger.hh regarding
# broken strict aliasing rules:
#
#   ../../src/parse/parsetiger.hh:
#     In instantiation of 'T& parse::variant<S>::as()
#     [with T = ast::ChunkList*; long unsigned int S = 16ul]':
#   ../../src/parse/parsetiger.hh:146:7:
#     required from 'void parse::variant<S>::swap(parse::variant<S>&)
#     [with T = ast::ChunkList*; long unsigned int S = 16ul]'
#   ../../src/parse/parsetiger.hh:156:7:
#     required from 'void parse::variant<S>::build(parse::variant<S>&)
#     [with T = ast::ChunkList*; long unsigned int S = 16ul]'
#   ../../src/parse/parsetiger.hh:872:46:   required from here
#   ../../src/parse/parsetiger.hh:130:46:
#     warning: dereferencing type-punned pointer will break strict-aliasing
#     rules [-Wstrict-aliasing]
#
#   ...
#
# Check whether the C++ compiler supports `-Wno-strict-aliasing' and
# if so, add it to NO_STRICT_ALIASING_CXXFLAGS and let Makefiles use
# it to compile code that include src/parse/parsetiger.hh.
TC_CXX_FLAGS([[-Wno-strict-aliasing]], [NO_STRICT_ALIASING_CXXFLAGS])


## -------------------- ##
## Auxiliary programs.  ##
## -------------------- ##

TC_PROG([reflex], [>= 3.2.10], [REFLEX], [Re/Flex scanner generator])

TC_PROG([bison], [>= 3.8.2], [BISON], [Bison parser generator])
AX_CONFIG_SCRIPTS([build-aux/bin/bison++])

# Compile Emacs files.
AM_PATH_LISPDIR

# Requested by Automake 1.12 for tcsh.
AM_PROG_AR

# We don't need shared libraries, speed the compilation up.
LT_INIT([disable-shared])

# Check for IA-32-GNU/Linux (and x86-64-GNU/Linux) to disable related
# parts on other platforms.
AM_CONDITIONAL([IA32_GNU_LINUX],
               [echo "$host" | grep -q '^\(i.86\|x86_64\).*linux'])
# Check for IA-32-FreeBSD to disable related parts on other platforms.
AM_CONDITIONAL([IA32_FREEBSD],
               [echo "$host" | grep -q '^\(i.86\|amd64\).*freebsd'])
# If one the previous IA-32 conditionals was true, enable SUPPORTED_IA32.
AM_CONDITIONAL([SUPPORTED_IA32],
               [echo "$host" | grep -q '^\(i.86\|x86_64\).*linux\|^\(i.86\|amd64\).*freebsd'])


# Debian does not provide a llvm-config binary
TC_PROG([llvm-config-18 llvm-config], [>=TC_CLANG_PREREQ], [LLVM_CONFIG], [Print LLVM compilation options])
TC_PROG([clang clang-18],             [>=TC_CLANG_PREREQ], [CLANG],       [C language family frontend for LLVM])

# Add a configure option to link LLVM statically
AC_ARG_ENABLE([static-llvm],
              [AS_HELP_STRING([--enable-static-llvm],
              [enable the linking statically against the LLVM back-end @<:@no@:>@])])

# Set STATIC_LLVM when linking statically
AM_CONDITIONAL([STATIC_LLVM], [test x"$enable_static_llvm" = x"yes"])



# Clang-format.
AC_CHECK_PROGS([CLANG_FORMAT], [clang-format])

## ----------- ##
## Libraries.  ##
## ----------- ##

BOOST_REQUIRE([1.61], [AC_MSG_ERROR([Boost is required])])
BOOST_CONVERSION # boost::lexical_cast
BOOST_GRAPH
BOOST_PROGRAM_OPTIONS
BOOST_STRING_ALGO
BOOST_SYSTEM

## ------ ##
## Tcsh.  ##
## ------ ##

# Check for Darwin to help Libtool use the right dynamic module
# extension on this system.
AM_CONDITIONAL([DARWIN], [echo "$host_os" | grep -q '^darwin'])
TC_WITH_TCSH([with_tcsh=yes], [with_tcsh=no])
AM_CONDITIONAL([ENABLE_TCSH], [test x$with_tcsh = xyes])
if test x$with_tcsh = xyes; then
  AC_DEFINE([WITH_SWIG], [1], [Define if building with swig.])
else
  AC_DEFINE([WITH_SWIG], [0], [Define if building with swig.])
fi

AC_CONFIG_FILES([tcsh/Makefile tcsh/python/Makefile])

# The Doxyfile is directly generated via `sed' (see doc/local.am)
# AC_CONFIG_FILES([doc/Doxyfile])

AX_CONFIG_SCRIPTS([tcsh/run])

## ------- ##
## Tests.  ##
## ------- ##

# Students should create their test suite in tests/.  Add the needed
# configure options here.
AC_CONFIG_FILES([tests/testsuite.sh], [chmod +x tests/testsuite.sh])



## --------------- ##
## Configuration.  ##
## --------------- ##

# Ask for the creation of config.h.
AC_CONFIG_HEADERS([config.h])

# Ask for the creation of the Makefiles.
AC_CONFIG_FILES([Makefile])

# Instantiate the output files.
AC_OUTPUT
