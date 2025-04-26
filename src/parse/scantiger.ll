                    /* -*- C++ -*- */
// %option defines the parameters with which the reflex will be launched
%option noyywrap
// To enable compatibility with bison
%option bison-complete
%option bison-cc-parser=parser
%option bison_cc_namespace=parse
%option bison-locations

%option lex=lex
// Add a param of function lex() generate in Lexer class
%option params="::parse::TigerDriver& td"
%option namespace=parse
// Name of the class generate by reflex
%option lexer=Lexer

%top{

#define YY_EXTERN_C extern "C" // For linkage rule

#include <cerrno>
#include <climits>
#include <regex>
#include <string>

#include <boost/lexical_cast.hpp>

#include <misc/contract.hh>
  // Using misc::escape is very useful to quote non printable characters.
  // For instance
  //
  //    std::cerr << misc::escape('\n') << '\n';
  //
  // reports about `\n' instead of an actual new-line character.
#include <misc/escape.hh>
#include <misc/symbol.hh>
#include <parse/parsetiger.hh>
#include <parse/tiger-driver.hh>

    // FIXED: Some code linked to location_ was deleted here.
#define YY_USER_ACTION       \
    do {                     \
      td.location_.step();   \
      for (size_t i = 0; i < yyleng; ++i) {               \
        if (yytext[i] == '\r' || yytext[i] == '\n') {     \
          if (i + 1 < yyleng &&                                   \
              ((yytext[i] == '\r' && yytext[i+1] == '\n')         \
               || (yytext[i] == '\n' && yytext[i+1] == '\r'))) {  \
            td.location_.lines();                                \
              ++i;                                               \
          } else {                                               \
            td.location_.lines();                                \
          }                                                      \
        } else {                                                 \
          td.location_.columns();                                \
        }                                                        \
    }                                                            \
} while (false);

#define TOKEN(Type)                             \
  parser::make_ ## Type(td.location_)

#define TOKEN_VAL(Type, Value)                  \
  parser::make_ ## Type(Value, td.location_)

# define CHECK_EXTENSION()                              \
  do {                                                  \
    if (!td.enable_extensions_p_)                       \
      td.error_ << misc::error::error_type::scan        \
                << td.location_ << ": "                 \
                << "invalid identifier: `"            \
                << misc::escape(text()) << "'\n";       \
  } while (false)
%}


// FIXED: Missing states needed.
/* States */
%x SC_COMMENT SC_STRING


// FIXED: Abbrevations missing.
/* Abbreviations.  */
space           [ \t]
eol             (\n\r|\r\n|\r|\n)
integer         [0-9]+
identifier      [a-zA-Z]([a-zA-Z]|[0-9]|"_")*|"_main"|"_hi"|"_lo"

hexa            \\x[0-9a-fA-F]{2}
octa            \\[0-3][0-7][0-7]

esc_backsl      \\\\
esc_quote       \\\"

comm_beg        "/*"
comm_end        "*/"

str_car         [^\"\\]


// FIXED: Need local variables here.
/* Local variables */
%class{
    int depth_comments;
    std::string str_buffer;
}

%%


// FIXED: Missing rules.
/* The rules */
/* <-- INITIAL STATE --> */
<INITIAL><<EOF>> { return TOKEN(EOF); }

<INITIAL>{integer} {
    long val = std::strtol(yytext, nullptr, 10);
    if (val < 0 || val > 2147483647)
    {
        td.error_ << misc::error::error_type::scan
                  << td.location_ << ": "
                  << "integer overflow or invalid value: `"
                  << misc::escape(text()) << "'\n";
        return TOKEN(PARSEerror);
    }
    return TOKEN_VAL(INT, static_cast<int>(val));
}

<INITIAL>array          { return TOKEN(ARRAY); }
<INITIAL>break          { return TOKEN(BREAK); }
<INITIAL>class          { return TOKEN(CLASS); }
<INITIAL>extends        { return TOKEN(EXTENDS); }
<INITIAL>method         { return TOKEN(METHOD); }
<INITIAL>new            { return TOKEN(NEW); }
<INITIAL>nil            { return TOKEN(NIL); }
<INITIAL>primitive      { return TOKEN(PRIMITIVE); }
<INITIAL>function       { return TOKEN(FUNCTION); }
<INITIAL>import         { return TOKEN(IMPORT); }
<INITIAL>type           { return TOKEN(TYPE); }
<INITIAL>var            { return TOKEN(VAR); }
<INITIAL>for            { return TOKEN(FOR); }
<INITIAL>if             { return TOKEN(IF); }
<INITIAL>in             { return TOKEN(IN); }
<INITIAL>while          { return TOKEN(WHILE); }
<INITIAL>else           { return TOKEN(ELSE); }
<INITIAL>end            { return TOKEN(END); }
<INITIAL>do             { return TOKEN(DO); }
<INITIAL>of             { return TOKEN(OF); }
<INITIAL>then           { return TOKEN(THEN); }
<INITIAL>to             { return TOKEN(TO); }
<INITIAL>let            { return TOKEN(LET); }
<INITIAL>_cast          { CHECK_EXTENSION(); return TOKEN(CAST); }
<INITIAL>_chunks        { CHECK_EXTENSION(); return TOKEN(CHUNKS); }
<INITIAL>_namety        { CHECK_EXTENSION(); return TOKEN(NAMETY); }
<INITIAL>_exp           { CHECK_EXTENSION(); return TOKEN(EXPMETA); }
<INITIAL>_lvalue        { CHECK_EXTENSION(); return TOKEN(LVALMETA); }

<INITIAL>{identifier}   { return TOKEN_VAL(ID, yytext); }

<INITIAL>"&"            { return TOKEN(AND); }
<INITIAL>":="           { return TOKEN(ASSIGN); }
<INITIAL>":"            { return TOKEN(COLON); }
<INITIAL>","            { return TOKEN(COMMA); }
<INITIAL>"/"            { return TOKEN(DIVIDE); }
<INITIAL>"."            { return TOKEN(DOT); }
<INITIAL>"{"            { return TOKEN(LBRACE); }
<INITIAL>"}"            { return TOKEN(RBRACE); }
<INITIAL>"["            { return TOKEN(LBRACK); }
<INITIAL>"]"            { return TOKEN(RBRACK); }
<INITIAL>"("            { return TOKEN(LPAREN); }
<INITIAL>")"            { return TOKEN(RPAREN); }
<INITIAL>";"            { return TOKEN(SEMI); }
<INITIAL>"="            { return TOKEN(EQ); }
<INITIAL>">="           { return TOKEN(GE); }
<INITIAL>">"            { return TOKEN(GT); }
<INITIAL>"<="           { return TOKEN(LE); }
<INITIAL>"<>"           { return TOKEN(NE); }
<INITIAL>"<"            { return TOKEN(LT); }
<INITIAL>"+"            { return TOKEN(PLUS); }
<INITIAL>"-"            { return TOKEN(MINUS); }
<INITIAL>"*"            { return TOKEN(TIMES); }
<INITIAL>"|"            { return TOKEN(OR); }

<INITIAL>{space}+ {}
<INITIAL>{eol} {}


<INITIAL>"\"" {
    str_buffer.clear();
    td.location_.columns(1);
    start(SC_STRING);
}

<INITIAL>{comm_beg} {
    depth_comments = 1;
    start(SC_COMMENT);
}


<INITIAL>. {
    td.error_ << misc::error::error_type::scan
              << td.location_ << ": "
              << "syntax error: `"
              << misc::escape(text()) << "'\n";
    return TOKEN(PARSEUNDEF);
}


/* <-- SC_STRING STATE --> */
<SC_STRING><<EOF>> {
    td.error_ << misc::error::error_type::scan
              << td.location_ << ": "
              << "unexpected end of file in a string\n";
    return TOKEN(PARSEerror);
}

<SC_STRING>"\"" {
    start(INITIAL);
    return TOKEN_VAL(STRING, str_buffer);
}

<SC_STRING>\\a { str_buffer += '\a'; }
<SC_STRING>\\b { str_buffer += '\b'; }
<SC_STRING>\\f { str_buffer += '\f'; }
<SC_STRING>\\n { str_buffer += '\n'; }
<SC_STRING>\\r { str_buffer += '\r'; }
<SC_STRING>\\t { str_buffer += '\t'; }
<SC_STRING>\\v { str_buffer += '\v'; }

<SC_STRING>{octa} {
    int car_val = std::stoi(yytext + 1, nullptr, 8);
    str_buffer += static_cast<char>(car_val);
}

<SC_STRING>{hexa} {
    char car = static_cast<char>(std::stoi(yytext + 2, nullptr, 16));
    str_buffer += car;
}

<SC_STRING>{esc_backsl} { str_buffer += "\\"; }
<SC_STRING>{esc_quote}  { str_buffer += "\""; }

<SC_STRING>\n {
    str_buffer += '\n';
}

<SC_STRING>\\. {
    td.error_ << misc::error::error_type::scan
              << td.location_ << ": "
              << "invalid escape sequence: `"
              << misc::escape(text()) << "'\n";
    return TOKEN(PARSEUNDEF);
}

<SC_STRING>[^\\\"]+ { str_buffer += yytext; }

<SC_STRING>. {}


/* <-- SC_COMMENT STATE --> */
<SC_COMMENT><<EOF>> {
    td.error_ << misc::error::error_type::scan
              << td.location_ << ": "
              << "unexpected end of file in a comment\n";
    return TOKEN(PARSEerror);
}

<SC_COMMENT>{comm_beg} { depth_comments++; }

<SC_COMMENT>{comm_end} {
    depth_comments--;
    if (depth_comments == 0)
        start(INITIAL);
}

<SC_COMMENT>\n {}

<SC_COMMENT>. {}

%%
