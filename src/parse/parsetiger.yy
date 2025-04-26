                                // -*- C++ -*-
%require "3.8"
%language "c++"
// Set the namespace name to `parse', instead of `yy'.
%define api.prefix {parse}
%define api.namespace {parse}
%define api.parser.class {parser}
%define api.value.type variant
%define api.token.constructor

// We use a GLR parser because it is able to handle Shift-Reduce and
// Reduce-Reduce conflicts by forking and testing each way at runtime. GLR
// permits, by allowing few conflicts, more readable and maintainable grammars.
%glr-parser
%skeleton "glr2.cc"

// In TC, we expect the GLR to resolve one Shift-Reduce and zero Reduce-Reduce
// conflict at runtime. Use %expect and %expect-rr to tell Bison about it.
%expect 1
%expect-rr 0

%define parse.error verbose
%defines
%debug
// Prefix all the tokens with TOK_ to avoid colisions.
%define api.token.prefix {TOK_}

/* We use pointers to store the filename in the locations.  This saves
   space (pointers), time (no deep copy), but leaves the problem of
   deallocation.  This would be a perfect job for a misc::symbol
   object (passed by reference), however Bison locations require the
   filename to be passed as a pointer, thus forcing us to handle the
   allocation and deallocation of this object.

   Nevertheless, all is not lost: we can still use a misc::symbol
   object to allocate a flyweight (constant) string in the pool of
   symbols, extract it from the misc::symbol object, and use it to
   initialize the location.  The allocated data will be freed at the
   end of the program (see the documentation of misc::symbol and
   misc::unique).  */
%define api.filename.type {const std::string}
%locations

/*---------------------.
| Support for tokens.  |
`---------------------*/
%code requires
{
#include <string>
#include <misc/algorithm.hh>
#include <misc/separator.hh>
#include <misc/symbol.hh>
#include <parse/fwd.hh>

  // Pre-declare parse::parse to allow a ``reentrant'' parsing within
  // the parser.
  namespace parse
  {
    ast_type parse(Tweast& input);
  }
}

// The parsing context.
%param { ::parse::TigerDriver& td }
%parse-param { ::parse::Lexer& lexer }

%printer { yyo << $$; } <int> <std::string> <misc::symbol>;

%token <std::string>    STRING "string"
%token <misc::symbol>   ID     "identifier"
%token <misc::symbol>   RESERVED_ID     "reserved_identifier"
%token <int>            INT    "integer"


/*--------------------------------.
| Support for the non-terminals.  |
`--------------------------------*/

%code requires
{
# include <ast/fwd.hh>
// Provide the declarations of the following classes for the
// %destructor clauses below to work properly.
# include <ast/exp.hh>
# include <ast/var.hh>
# include <ast/ty.hh>
# include <ast/name-ty.hh>
# include <ast/field.hh>
# include <ast/field-init.hh>
# include <ast/function-dec.hh>
# include <ast/type-dec.hh>
# include <ast/var-dec.hh>
# include <ast/chunk.hh>
# include <ast/chunk-list.hh>
}

  // FIXME: Some code was deleted here (Printers and destructors).
%destructor { delete $$; } <ast::Exp*> <ast::Var*> <ast::Ty*> <ast::NameTy*> <ast::Field*> <ast::FieldInit*> <ast::FunctionDec*> <ast::TypeDec*> <ast::VarDec*> <ast::ChunkList*>

/*-----------------------------------------.
| Code output in the implementation file.  |
`-----------------------------------------*/

%code
{
# include <parse/tweast.hh>
# include <misc/separator.hh>
# include <misc/symbol.hh>
# include <ast/all.hh>
# include <ast/libast.hh>
# include <parse/tiger-factory.hh>
# include <parse/tiger-driver.hh>

  namespace
  {

    /// Get the metavar from the specified map.
    template <typename T>
    T*
    metavar(parse::TigerDriver& td, unsigned key)
    {
      parse::Tweast* input = td.input_;
      return input->template take<T>(key);
    }

  }
}

%code
{
  #include <parse/scantiger.hh>  // header file generated with reflex --header-file
  #undef yylex
  #define yylex lexer.lex  // Within bison's parse() we should invoke lexer.lex(), not the global lex()
}

// Definition of the tokens, and their pretty-printing.
%token AND          "&"
       ARRAY        "array"
       ASSIGN       ":="
       BREAK        "break"
       CAST         "_cast"
       CLASS        "class"
       COLON        ":"
       COMMA        ","
       DIVIDE       "/"
       DO           "do"
       DOT          "."
       ELSE         "else"
       END          "end"
       EQ           "="
       EXTENDS      "extends"
       EXPMETA      "_exp"
       FOR          "for"
       FUNCTION     "function"
       GE           ">="
       GT           ">"
       IF           "if"
       IMPORT       "import"
       IN           "in"
       LBRACE       "{"
       LBRACK       "["
       LE           "<="
       LET          "let"
       LPAREN       "("
       LT           "<"
       LVALMETA     "_lvalue"
       MINUS        "-"
       METHOD       "method"
       NE           "<>"
       NEW          "new"
       NIL          "nil"
       OF           "of"
       OR           "|"
       PLUS         "+"
       PRIMITIVE    "primitive"
       RBRACE       "}"
       RBRACK       "]"
       RPAREN       ")"
       SEMI         ";"
       THEN         "then"
       TIMES        "*"
       TO           "to"
       TYPE         "type"
       VAR          "var"
       WHILE        "while"
       EOF 0        "end of file"

%type <ast::Exp*>               exp
%type <ast::exps_type*>         exps exps.1
%type <ast::ChunkList*>         chunks

%type <ast::Exp*> exp_noassign

%type <ast::TypeChunk*>         tychunk
%type <ast::TypeDec*>           tydec
%type <ast::NameTy*>            typeid
%type <ast::Ty*>                ty

%type <ast::Field*>             tyfield
%type <ast::fields_type*>       tyfields tyfields.1
%type <ast::VarChunk*>          tyfields_varChunk tyfields_varChunk.1
%type <ast::VarDec*>            tyfield_varDec

%type <ast::fieldinits_type*>   tyinitfields tyinitfields.1
%type <ast::Var*>               lvalue


%type <ast::NameTy*>            optional_typeid

%type <ast::FunctionChunk*>     funchunk
%type <ast::VarChunk*>          varchunk varchunk.1

%type <ast::FunctionDec*>       fundec
%type <ast::VarDec*>            vardec
%type <ast::ChunkList*>         importchunks

%type <ast::NameTy*>            optional_colon_typeid
%type <ast::exps_type*> args args.1

  // FIXED: Some code was deleted here (Priorities/associativities).

%left OR
%left AND
%nonassoc EQ NE LT GT LE GE
%nonassoc THEN
%nonassoc ELSE
%left PLUS MINUS
%left TIMES DIVIDE
%precedence DO ASSIGN OF



// Solving conflicts on:
// let type foo = bar
//     type baz = bat
// which can be understood as a list of two TypeChunk containing
// a unique TypeDec each, or a single TypeChunk containing two TypeDec.
// We want the latter.
%precedence CHUNKS
%precedence TYPE
  // FIXED: Some code was deleted here (Other declarations).
%precedence FUNCTION
%precedence PRIMITIVE


%start program

%%
program:
  /* Parsing a source program.  */
  exp
   { td.ast_ = $1; }
| /* Parsing an imported file.  */
  chunks
   { td.ast_ = $1; }
;

  // FIXED: Some code was deleted here (More rules).



exps:
  %empty                { $$ = make_exps_type(); }
| exp exps.1            { $$ = $2; $$->insert($$->begin(), $1); }

exps.1:
  %empty                { $$ = make_exps_type(); }
| SEMI exp exps.1        { $$ = $3; $$->insert($$->begin(), $2); }

exp:
    exp_noassign
  | ID LBRACK exp RBRACK OF exp
    { $$ = make_ArrayExp(@$, make_NameTy(@$, $1), $3, $6); }
  | lvalue ASSIGN exp
      { $$ = make_AssignExp(@$, $1, $3); }
  | IF exp THEN exp %prec THEN
      { $$ = make_IfExp(@$, $2, $4); }
  | IF exp THEN exp ELSE exp
      { $$ = make_IfExp(@$, $2, $4, $6); }
  | WHILE exp DO exp
      { $$ = make_WhileExp(@$, $2, $4); }
  | FOR ID ASSIGN exp TO exp DO exp
      { $$ = make_ForExp(@$, make_VarDec(@2, $2, nullptr, $4), $6, $8); }
  | BREAK
      { $$ = make_BreakExp(@$); }
  | LET chunks IN exps END
      { $$ = make_LetExp(@$, $2, make_SeqExp(@$, $4)); }
  | CAST LPAREN exp COMMA ty RPAREN
      { $$ = make_CastExp(@$, $3, $5); }
  | EXPMETA LPAREN INT RPAREN
      { $$ = metavar<ast::Exp>(td, $3); }
;



exp_noassign:
    NIL               { $$ = make_NilExp(@$); }
  | INT               { $$ = make_IntExp(@$, $1); }
  | STRING            { $$ = make_StringExp(@$, $1); }
  | lvalue            { $$ = $1; }
  | ID LPAREN args RPAREN
      { $$ = make_CallExp(@$, $1, $3); }
  | ID LBRACE tyinitfields RBRACE
      { $$ = make_RecordExp(@$, make_NameTy(@$, $1), $3); }
  | MINUS exp_noassign
      { $$ = make_OpExp(@$, make_IntExp(@$, 0), ast::OpExp::Oper::sub, $2); }
  | exp_noassign TIMES exp_noassign
      { $$ = make_OpExp(@$, $1, ast::OpExp::Oper::mul, $3); }
  | exp_noassign DIVIDE exp_noassign
      { $$ = make_OpExp(@$, $1, ast::OpExp::Oper::div, $3); }
  | exp_noassign PLUS exp_noassign
      { $$ = make_OpExp(@$, $1, ast::OpExp::Oper::add, $3); }
  | exp_noassign MINUS exp_noassign
      { $$ = make_OpExp(@$, $1, ast::OpExp::Oper::sub, $3); }
  | exp_noassign EQ exp_noassign
      { $$ = make_OpExp(@$, $1, ast::OpExp::Oper::eq, $3); }
  | exp_noassign NE exp_noassign
      { $$ = make_OpExp(@$, $1, ast::OpExp::Oper::ne, $3); }
  | exp_noassign LT exp_noassign
      { $$ = make_OpExp(@$, $1, ast::OpExp::Oper::lt, $3); }
  | exp_noassign LE exp_noassign
      { $$ = make_OpExp(@$, $1, ast::OpExp::Oper::le, $3); }
  | exp_noassign GT exp_noassign
      { $$ = make_OpExp(@$, $1, ast::OpExp::Oper::gt, $3); }
  | exp_noassign GE exp_noassign
      { $$ = make_OpExp(@$, $1, ast::OpExp::Oper::ge, $3); }
  | exp_noassign AND exp_noassign
      { $$ = parse::parse(parse::Tweast()
            << "("
            << "if (" << $1 << ") then ((" << $3 << ") <> 0) else 0"
            << ")" ); }
  | exp_noassign OR exp_noassign
      { $$ = parse::parse(parse::Tweast()
            << "("
            << "if (" << $1 << ") then 1 else ((" << $3 << ") <> 0)"
            << ")" ); }
  | LPAREN exps RPAREN
      { $$ = make_SeqExp(@$, $2); }





tyinitfields:
  %empty                        { $$ = make_fieldinits_type(); }
| ID EQ exp tyinitfields.1       { $$ = $4; $$->insert($$->begin(), make_FieldInit(@$, $1, $3)); }

tyinitfields.1:
  %empty                        { $$ = make_fieldinits_type(); }
| COMMA ID EQ exp tyinitfields.1   { $$ = $5; $$->insert($$->begin(), make_FieldInit(@$, $2, $4)); }
;


lvalue:
  ID                  { $$ = make_SimpleVar(@$, $1); }
| lvalue DOT ID       { $$ = make_FieldVar(@$, $1, $3); }
| lvalue LBRACK exp RBRACK  { $$ = make_SubscriptVar(@$, $1, $3); }
| LVALMETA LPAREN INT RPAREN { $$ = metavar<ast::Var>(td, $3); }
;


args:
  %empty         { $$ = make_exps_type(); }
| exp args.1     { $$ = $2; $$->insert($$->begin(), $1); }

args.1:
  %empty                     { $$ = make_exps_type(); }
| COMMA exp args.1           { $$ = $3; $$->insert($$->begin(), $2); }





/*---------------.
| Declarations.  |
`---------------*/

%token CHUNKS "_chunks";

  // FIXED: Some code was deleted here (More rules) until the type declarations box.
chunks:
  /* Chunks are contiguous series of declarations of the same type
     (TypeDec, FunctionDec...) to which we allow certain specfic behavior like
     self referencing.
     They are held by a ChunkList, that can be empty, like in this case:
        let
        in
            ..
        end
     which is why we end the recursion with a %empty. */
  %empty
    { $$ = make_ChunkList(@$); }
| tychunk chunks
    { $$ = $2; $$->push_front($1); }
| funchunk chunks
    { $$ = $2; $$->push_front($1); }
| varchunk chunks
    { $$ = $2; $$->push_front($1); }
| importchunks chunks
    { $$ = $2; for (auto chunk : *$1) {$$->push_front(chunk); } }
| CHUNKS LPAREN INT RPAREN chunks
    { $$ = metavar<ast::ChunkList>(td, $3); $$->splice_back(*$5); }
;

funchunk:
  fundec %prec CHUNKS
    { $$ = make_FunctionChunk(@$); $$->push_front(*$1); }
| fundec funchunk
    { $$ = $2; $$->push_front(*$1); }
;


varchunk:
    vardec %prec CHUNKS { $$ = make_VarChunk(@1); $$->push_front(*$1); }
;


fundec:
  FUNCTION ID LPAREN tyfields_varChunk RPAREN optional_typeid EQ exp
    { $$ = make_FunctionDec(@$, $2, $4, $6, $8); }
| PRIMITIVE ID LPAREN tyfields_varChunk RPAREN optional_typeid
    { $$ = make_FunctionDec(@$, $2, $4, $6, nullptr); }
;

optional_typeid:
  %empty
    { $$ = nullptr; }
| COLON typeid
    { $$ = $2; }
;

tyfields_varChunk:
  %empty
    { $$ = make_VarChunk(@$); }
| tyfields_varChunk.1
    { $$ = $1; }
;

tyfields_varChunk.1:
  tyfields_varChunk.1 COMMA tyfield_varDec
    { $$ = $1; $$->emplace_back(*$3); }
| tyfield_varDec
    { $$ = make_VarChunk(@$); $$->emplace_back(*$1); }
;

tyfield_varDec:
  ID COLON typeid     { $$ = make_VarDec(@$, $1, $3, nullptr); }
;

vardec:
  VAR ID optional_colon_typeid ASSIGN exp
    { $$ = make_VarDec(@$, $2, $3, $5); }
;

optional_colon_typeid:
  %empty
    { $$ = nullptr; }
| COLON typeid
    { $$ = $2; }
;

importchunks:
  IMPORT STRING { $$ = td.parse_import($2, @$); }
;



/*--------------------.
| Type Declarations.  |
`--------------------*/

tychunk:
  /* Use `%prec CHUNKS' to do context-dependent precedence and resolve a
     shift-reduce conflict. */
  tydec %prec CHUNKS  { $$ = make_TypeChunk(@$); $$->push_front(*$1); }
| tydec tychunk       { $$ = $2; $$->push_front(*$1); }
;

tydec:
  TYPE ID EQ ty { $$ = make_TypeDec(@$, $2, $4); }
;

ty:
  typeid               { $$ = $1; }
| LBRACE tyfields RBRACE     { $$ = make_RecordTy(@$, $2); }
| ARRAY OF typeid  { $$ = make_ArrayTy(@$, $3); }
;

tyfields:
  %empty               { $$ = make_fields_type(); }
| tyfields.1           { $$ = $1; }
;

tyfields.1:
  tyfields.1 COMMA tyfield { $$ = $1; $$->emplace_back($3); }
| tyfield                { $$ = make_fields_type($1); }
;

tyfield:
  ID COLON typeid     { $$ = make_Field(@$, $1, $3); }
;

%token NAMETY "_namety";
typeid:
  ID                    { $$ = make_NameTy(@$, $1); }
  /* This is a metavariable. It is used internally by TWEASTs to retrieve
     already parsed nodes when given an input to parse. */
| NAMETY LPAREN INT RPAREN    { $$ = metavar<ast::NameTy>(td, $3); }
;


%%

void
parse::parser::error(const location_type& l, const std::string& m)
{
    // FIXED: Deleted code here.
    td.error_ << misc::error::error_type::parse << l << ": " << m << std::endl;
}


