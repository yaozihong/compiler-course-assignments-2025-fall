%{
  #include <iostream>
  #include "cool-tree.h"
  #include "stringtab.h"
  #include "utilities.h"
  
  extern char *curr_filename;
  extern int curr_lineno;
  extern Symbol self_sym;
  
  /* Locations */
  #define YYLTYPE int
  #define cool_yylloc curr_lineno
  
  extern int node_lineno;
  #define YYLLOC_DEFAULT(Current, Rhs, N) \
  Current = Rhs[1]; \
  node_lineno = Current;
  
  #define SET_NODELOC(Current)  \
  node_lineno = Current;
  
  void yyerror(char *s);
  extern int cool_yylex();
  
  Program ast_root;
  Classes parse_results;
  int omerrs = 0;
    %}

    %union {
      Boolean boolean;
      Symbol symbol;
      Program program;
      Class_ class_;
      Classes classes;
      Feature feature;
      Features features;
      Formal formal;
      Formals formals;
      Case case_;
      Cases cases;
      Expression expression;
      Expressions expressions;
      char *error_msg;
    }

    %token CLASS 258 ELSE 259 FI 260 IF 261 IN 262 
    %token INHERITS 263 LET 264 LOOP 265 POOL 266 THEN 267 WHILE 268
    %token CASE 269 ESAC 270 OF 271 DARROW 272 NEW 273 ISVOID 274
    %token <symbol>  STR_CONST 275 INT_CONST 276 
    %token <boolean> BOOL_CONST 277
    %token <symbol>  TYPEID 278 OBJECTID 279 
    %token ASSIGN 280 NOT 281 LE 282 ERROR 283
  
    %type <program> program
    %type <classes> class_list
    %type <class_> class
    
    %type <features> feature_list
    %type <feature> feature
    %type <formals> formal_list
    %type <formal> formal
    %type <cases> case_list
    %type <case_> case_branch
    %type <expressions> expr_list
    %type <expressions> block_expr_list
    %type <expression> expr
    %type <expression> let_expr
    %type <expression> opt_init
    
    %start program
    
    %nonassoc IN
    %right ASSIGN
    %right NOT
    %nonassoc LE '<' '='
    %left '+' '-'
    %left '*' '/'
    %left ISVOID
    %left '~'
    %left '@'
    %left '.'
    
    %%
    program	: class_list	{ @$ = @1; ast_root = program($1); }
    ;
    
    class_list
    : class
    { $$ = single_Classes($1);
    parse_results = $$; }
    | class_list class
    { $$ = append_Classes($1,single_Classes($2)); 
    parse_results = $$; }
    ;
    
    class	: CLASS TYPEID '{' feature_list '}' ';'
    { $$ = class_($2,idtable.add_string("Object"),$4,
    stringtable.add_string(curr_filename)); }
    | CLASS TYPEID INHERITS TYPEID '{' feature_list '}' ';'
    { $$ = class_($2,$4,$6,stringtable.add_string(curr_filename)); }
    ;
    feature_list
    : /* empty */
    {  $$ = nil_Features(); }
    | feature_list feature ';'
    {  $$ = append_Features($1, single_Features($2)); }
    | feature_list error ';'
    {  $$ = $1; yyerrok; }
    ;
    
    feature
    : OBJECTID '(' formal_list ')' ':' TYPEID '{' expr '}'
    { @$ = @1; SET_NODELOC(@1); $$ = method($1, $3, $6, $8); }
    | OBJECTID ':' TYPEID opt_init
    { @$ = @1; SET_NODELOC(@1); $$ = attr($1, $3, $4); }
    ;
    
    formal_list
    : /* empty */
    { $$ = nil_Formals(); }
    | formal
    { $$ = single_Formals($1); }
    | formal_list ',' formal
    { $$ = append_Formals($1, single_Formals($3)); }
    ;
    
    formal
    : OBJECTID ':' TYPEID
    { @$ = @1; SET_NODELOC(@1); $$ = formal($1, $3); }
    ;
    
    opt_init
    : ASSIGN expr
    { @$ = @2; $$ = $2; }
    | /* empty */
    { @$ = @0; SET_NODELOC(@$); $$ = no_expr(); }
    ;
    
    expr_list
    : /* empty */
    { $$ = nil_Expressions(); }
    | expr
    { $$ = single_Expressions($1); }
    | expr_list ',' expr
    { $$ = append_Expressions($1, single_Expressions($3)); }
    ;
    
    block_expr_list
    : expr ';'
    { $$ = single_Expressions($1); }
    | block_expr_list expr ';'
    { $$ = append_Expressions($1, single_Expressions($2)); }
    ;
    
    case_list
    : case_branch
    { $$ = single_Cases($1); }
    | case_list case_branch
    { $$ = append_Cases($1, single_Cases($2)); }
    ;
    
    case_branch
    : OBJECTID ':' TYPEID DARROW expr ';'
    { @$ = @1; SET_NODELOC(@1); $$ = branch($1, $3, $5); }
    ;
    
    let_expr
    : OBJECTID ':' TYPEID opt_init IN expr
    { @$ = @1; SET_NODELOC(@1); $$ = let($1, $3, $4, $6); }
    | OBJECTID ':' TYPEID opt_init ',' let_expr
    { @$ = @1; SET_NODELOC(@1); $$ = let($1, $3, $4, $6); }
    ;
    
    expr
    : OBJECTID ASSIGN expr
    { @$ = @1; SET_NODELOC(@1); $$ = assign($1, $3); }
    | expr '@' TYPEID '.' OBJECTID '(' expr_list ')'
    { @$ = @1; SET_NODELOC(@1); $$ = static_dispatch($1, $3, $5, $7); }
    | expr '.' OBJECTID '(' expr_list ')'
    { @$ = @1; SET_NODELOC(@1); $$ = dispatch($1, $3, $5); }
    | OBJECTID '(' expr_list ')'
    { @$ = @1; SET_NODELOC(@1); $$ = dispatch(object(self_sym), $1, $3); }
    | IF expr THEN expr ELSE expr FI
    { @$ = @1; SET_NODELOC(@1); $$ = cond($2, $4, $6); }
    | WHILE expr LOOP expr POOL
    { @$ = @1; SET_NODELOC(@1); $$ = loop($2, $4); }
    | '{' block_expr_list '}'
    { @$ = @1; SET_NODELOC(@1); $$ = block($2); }
    | LET let_expr
    { $$ = $2; }
    | CASE expr OF case_list ESAC
    { @$ = @1; SET_NODELOC(@1); $$ = typcase($2, $4); }
    | NEW TYPEID
    { @$ = @1; SET_NODELOC(@1); $$ = new_($2); }
    | ISVOID expr
    { @$ = @1; SET_NODELOC(@1); $$ = isvoid($2); }
    | expr '+' expr
    { @$ = @1; SET_NODELOC(@1); $$ = plus($1, $3); }
    | expr '-' expr
    { @$ = @1; SET_NODELOC(@1); $$ = sub($1, $3); }
    | expr '*' expr
    { @$ = @1; SET_NODELOC(@1); $$ = mul($1, $3); }
    | expr '/' expr
    { @$ = @1; SET_NODELOC(@1); $$ = divide($1, $3); }
    | '~' expr
    { @$ = @1; SET_NODELOC(@1); $$ = neg($2); }
    | expr '<' expr
    { @$ = @1; SET_NODELOC(@1); $$ = lt($1, $3); }
    | expr LE expr
    { @$ = @1; SET_NODELOC(@1); $$ = leq($1, $3); }
    | expr '=' expr
    { @$ = @1; SET_NODELOC(@1); $$ = eq($1, $3); }
    | NOT expr
    { @$ = @1; SET_NODELOC(@1); $$ = comp($2); }
    | '(' expr ')'
    { $$ = $2; }
    | OBJECTID
    { @$ = @1; SET_NODELOC(@1); $$ = object($1); }
    | INT_CONST
    { @$ = @1; SET_NODELOC(@1); $$ = int_const($1); }
    | STR_CONST
    { @$ = @1; SET_NODELOC(@1); $$ = string_const($1); }
    | BOOL_CONST
    { @$ = @1; SET_NODELOC(@1); $$ = bool_const($1); }
    ;
    
%%

    /* Initialize the shared line counter once before parsing. */
    static int curr_lineno_init = (curr_lineno = 1);
    Symbol self_sym = idtable.add_string("self");

    void yyerror(char *s)
    {
      extern int curr_lineno;
      
      cerr << "\"" << curr_filename << "\", line " << curr_lineno << ": " \
      << s << " at or near ";
      print_cool_token(yychar);
      cerr << endl;
      omerrs++;
      
      if(omerrs>50) {fprintf(stdout, "More than 50 errors\n"); exit(1);}
    }
