#ifndef SEMANT_H_
#define SEMANT_H_

#include <assert.h>
#include <iostream>  
#include "cool-tree.h"
#include "stringtab.h"
#include "symtab.h"
#include "list.h"

#define TRUE 1
#define FALSE 0

class ClassTable;
typedef ClassTable *ClassTableP;

// This is a structure that may be used to contain the semantic
// information such as the inheritance graph.  You may use it or not as
// you like: it is only here to provide a container for the supplied
// methods.

class ClassTable {
 private:
  int semant_errors_;
  ostream& error_stream_;
  SymbolTable<Symbol, Class_> *class_table_;
  Class_ Object_class_;
  Class_ IO_class_;
  Class_ Int_class_;
  Class_ Bool_class_;
  Class_ Str_class_;

  void install_basic_classes();
  void build_inheritance_graph(Classes classes);
  void check_inheritance(Classes classes);
  void check_class_inheritance(Class_ c);
  void check_features(Classes classes);
  void check_class_features(Class_ c);
  void add_inherited_attrs(Class_ c, SymbolTable<Symbol, Symbol> *env);
 public:
  ClassTable(Classes);
  int errors() { return semant_errors_; }
  ostream& semant_error();
  ostream& semant_error(Class_ c);
  ostream& semant_error(Symbol filename, tree_node *t);
  method_class *lookup_method(Symbol class_name, Symbol method_name);
  Class_ lookup_class(Symbol class_name);
  Symbol resolve_type(Symbol type, Class_ current_class);
  bool is_subtype(Symbol child, Symbol parent, Class_ current_class);
  Symbol lub(Symbol a, Symbol b, Class_ current_class);
};


#endif
