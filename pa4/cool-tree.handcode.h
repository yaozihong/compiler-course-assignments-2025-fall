//
// The following include files must come first.

#ifndef COOL_TREE_HANDCODE_H
#define COOL_TREE_HANDCODE_H

#include <iostream>
#include "tree.h"
#include "cool.h"
#include "stringtab.h"
#include "symtab.h"
#define yylineno curr_lineno;
extern int yylineno;

inline Boolean copy_Boolean(Boolean b) {return b; }
inline void assert_Boolean(Boolean) {}
inline void dump_Boolean(ostream& stream, int padding, Boolean b)
	{ stream << pad(padding) << (int) b << "\n"; }

void dump_Symbol(ostream& stream, int padding, Symbol b);
void assert_Symbol(Symbol b);
Symbol copy_Symbol(Symbol b);

class Program_class;
typedef Program_class *Program;
class Class__class;
typedef Class__class *Class_;
class ClassTable;
class Feature_class;
typedef Feature_class *Feature;
class Formal_class;
typedef Formal_class *Formal;
class Expression_class;
typedef Expression_class *Expression;
class Case_class;
typedef Case_class *Case;

typedef list_node<Class_> Classes_class;
typedef Classes_class *Classes;
typedef list_node<Feature> Features_class;
typedef Features_class *Features;
typedef list_node<Formal> Formals_class;
typedef Formals_class *Formals;
typedef list_node<Expression> Expressions_class;
typedef Expressions_class *Expressions;
typedef list_node<Case> Cases_class;
typedef Cases_class *Cases;

#define Program_EXTRAS                          \
virtual void semant() = 0;			\
virtual void dump_with_types(ostream&, int) = 0; 



#define program_EXTRAS                            \
void semant();     				                        \
void dump_with_types(ostream&, int);            

#define Class__EXTRAS                             \
virtual Symbol get_filename() = 0;                \
virtual Symbol get_name() = 0;                    \
virtual Symbol get_parent() = 0;                  \
virtual Features get_features() = 0;              \
virtual void dump_with_types(ostream&,int) = 0; 


#define class__EXTRAS                                 \
Symbol get_filename() { return filename; }            \
Symbol get_name() { return name; }                    \
Symbol get_parent() { return parent; }                \
Features get_features() { return features; }          \
void dump_with_types(ostream&,int);                    


#define Feature_EXTRAS                                \
virtual Symbol get_name() = 0;                        \
virtual bool is_method() = 0;                         \
virtual bool is_attr() = 0;                           \
virtual void dump_with_types(ostream&,int) = 0; 


#define Feature_SHARED_EXTRAS                                       \
void dump_with_types(ostream&,int);    

#define method_EXTRAS                                               \
Symbol get_name() { return name; }                                  \
Formals get_formals() { return formals; }                           \
Symbol get_return_type() { return return_type; }                    \
Expression get_expr() { return expr; }                              \
bool is_method() { return true; }                                   \
bool is_attr() { return false; }

#define attr_EXTRAS                                                 \
Symbol get_name() { return name; }                                  \
Symbol get_type_decl() { return type_decl; }                        \
Expression get_init() { return init; }                              \
bool is_method() { return false; }                                  \
bool is_attr() { return true; }




#define Formal_EXTRAS                              \
virtual Symbol get_name() = 0;                     \
virtual Symbol get_type_decl() = 0;                \
virtual void dump_with_types(ostream&,int) = 0;


#define formal_EXTRAS                           \
Symbol get_name() { return name; }              \
Symbol get_type_decl() { return type_decl; }    \
void dump_with_types(ostream&,int);


#define Case_EXTRAS                             \
virtual Symbol get_name() = 0;                  \
virtual Symbol get_type_decl() = 0;             \
virtual Expression get_expr() = 0;              \
virtual void dump_with_types(ostream& ,int) = 0; 


#define branch_EXTRAS                                   \
Symbol get_name() { return name; }                      \
Symbol get_type_decl() { return type_decl; }            \
Expression get_expr() { return expr; }                  \
void dump_with_types(ostream& ,int);


#define Expression_EXTRAS                    \
Symbol type;                                 \
Symbol get_type() { return type; }           \
Expression set_type(Symbol s) { type = s; return this; } \
virtual Symbol type_check(ClassTable *classtable, Class_ current_class, \
                          SymbolTable<Symbol, Symbol> *object_env) = 0; \
virtual void dump_with_types(ostream&,int) = 0;  \
void dump_type(ostream&, int);               \
Expression_class() { type = (Symbol) NULL; }

#define Expression_SHARED_EXTRAS           \
void dump_with_types(ostream&,int); 

#define assign_EXTRAS                                              \
Symbol type_check(ClassTable *, Class_, SymbolTable<Symbol, Symbol> *);

#define static_dispatch_EXTRAS                                     \
Symbol type_check(ClassTable *, Class_, SymbolTable<Symbol, Symbol> *);

#define dispatch_EXTRAS                                            \
Symbol type_check(ClassTable *, Class_, SymbolTable<Symbol, Symbol> *);

#define cond_EXTRAS                                                \
Symbol type_check(ClassTable *, Class_, SymbolTable<Symbol, Symbol> *);

#define loop_EXTRAS                                                \
Symbol type_check(ClassTable *, Class_, SymbolTable<Symbol, Symbol> *);

#define typcase_EXTRAS                                             \
Symbol type_check(ClassTable *, Class_, SymbolTable<Symbol, Symbol> *);

#define block_EXTRAS                                               \
Symbol type_check(ClassTable *, Class_, SymbolTable<Symbol, Symbol> *);

#define let_EXTRAS                                                 \
Symbol type_check(ClassTable *, Class_, SymbolTable<Symbol, Symbol> *);

#define plus_EXTRAS                                                \
Symbol type_check(ClassTable *, Class_, SymbolTable<Symbol, Symbol> *);

#define sub_EXTRAS                                                 \
Symbol type_check(ClassTable *, Class_, SymbolTable<Symbol, Symbol> *);

#define mul_EXTRAS                                                 \
Symbol type_check(ClassTable *, Class_, SymbolTable<Symbol, Symbol> *);

#define divide_EXTRAS                                              \
Symbol type_check(ClassTable *, Class_, SymbolTable<Symbol, Symbol> *);

#define neg_EXTRAS                                                 \
Symbol type_check(ClassTable *, Class_, SymbolTable<Symbol, Symbol> *);

#define lt_EXTRAS                                                  \
Symbol type_check(ClassTable *, Class_, SymbolTable<Symbol, Symbol> *);

#define eq_EXTRAS                                                  \
Symbol type_check(ClassTable *, Class_, SymbolTable<Symbol, Symbol> *);

#define leq_EXTRAS                                                 \
Symbol type_check(ClassTable *, Class_, SymbolTable<Symbol, Symbol> *);

#define comp_EXTRAS                                                \
Symbol type_check(ClassTable *, Class_, SymbolTable<Symbol, Symbol> *);

#define int_const_EXTRAS                                           \
Symbol type_check(ClassTable *, Class_, SymbolTable<Symbol, Symbol> *);

#define bool_const_EXTRAS                                          \
Symbol type_check(ClassTable *, Class_, SymbolTable<Symbol, Symbol> *);

#define string_const_EXTRAS                                        \
Symbol type_check(ClassTable *, Class_, SymbolTable<Symbol, Symbol> *);

#define new__EXTRAS                                                \
Symbol type_check(ClassTable *, Class_, SymbolTable<Symbol, Symbol> *);

#define isvoid_EXTRAS                                              \
Symbol type_check(ClassTable *, Class_, SymbolTable<Symbol, Symbol> *);

#define no_expr_EXTRAS                                             \
Symbol type_check(ClassTable *, Class_, SymbolTable<Symbol, Symbol> *);

#define object_EXTRAS                                              \
Symbol type_check(ClassTable *, Class_, SymbolTable<Symbol, Symbol> *);

#endif
