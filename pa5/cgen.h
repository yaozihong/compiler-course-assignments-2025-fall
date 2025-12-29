#include <assert.h>
#include <stdio.h>
#include <map>
#include <vector>
#include "emit.h"
class CgenNode;
#define method_EXTRAS void code(ostream&, CgenNode*);
#include "cool-tree.h"
#include "symtab.h"

enum Basicness     {Basic, NotBasic};
#define TRUE 1
#define FALSE 0

class CgenClassTable;
typedef CgenClassTable *CgenClassTableP;

class CgenNode;
typedef CgenNode *CgenNodeP;

class Environment {
public:
   Environment() : m_class_node(NULL), temp_count(0) {}

   void EnterScope() { var_scope_sizes.push_back(vars.size()); }
   void ExitScope() {
      if (!var_scope_sizes.empty()) {
         vars.resize(var_scope_sizes.back());
         var_scope_sizes.pop_back();
      }
   }

   void AddVar(Symbol sym) { vars.push_back(sym); }
   void AddTemp() { ++temp_count; }
   int LookUpVar(Symbol sym) const {
      int idx = 0;
      for (int i = static_cast<int>(vars.size()) - 1; i >= 0; --i, ++idx) {
         if (vars[i] == sym) {
            return idx + temp_count;
         }
      }
      return -1;
   }

   void ClearParams() { params.clear(); }
   void AddParam(Symbol sym) { params.push_back(sym); }
   int LookUpParam(Symbol sym) const {
      for (int i = 0; i < static_cast<int>(params.size()); ++i) {
         if (params[i] == sym) {
            return static_cast<int>(params.size()) - 1 - i;
         }
      }
      return -1;
   }

   void SetAttrs(const std::vector<Symbol> &attrs_in) { attrs = attrs_in; }
   int LookUpAttrib(Symbol sym) const {
      for (int i = 0; i < static_cast<int>(attrs.size()); ++i) {
         if (attrs[i] == sym) {
            return i;
         }
      }
      return -1;
   }

   CgenNode *m_class_node;

private:
   std::vector<Symbol> vars;
   std::vector<int> var_scope_sizes;
   std::vector<Symbol> params;
   std::vector<Symbol> attrs;
   int temp_count;
};

class CgenClassTable : public SymbolTable<Symbol,CgenNode> {
private:
   List<CgenNode> *nds;
   ostream& str;
   int stringclasstag;
   int intclasstag;
   int boolclasstag;
   std::vector<CgenNode*> class_nodes;
   std::map<Symbol, int> class_tags;


// The following methods emit code for
// constants and global declarations.

   void code_global_data();
   void code_global_text();
   void code_bools(int);
   void code_select_gc();
   void code_constants();
   void code_class_name_tab();
   void code_class_obj_tab();
   void code_dispatch_tabs();
   void code_prot_objs();
   void code_class_inits();
   void code_class_methods();

// The following creates an inheritance graph from
// a list of classes.  The graph is implemented as
// a tree of `CgenNode', and class names are placed
// in the base class symbol table.

   void install_basic_classes();
   void install_class(CgenNodeP nd);
   void install_classes(Classes cs);
   void build_inheritance_tree();
   void set_relations(CgenNodeP nd);
public:
   CgenClassTable(Classes, ostream& str);
   void code();
   CgenNodeP root();
   std::vector<CgenNode*> get_class_nodes();
   const std::map<Symbol, int>& get_class_tags() const { return class_tags; }
   CgenNodeP get_class_node(Symbol sym) { return probe(sym); }
};


class CgenNode : public class__class {
private: 
   CgenNodeP parentnd;                        // Parent of class
   List<CgenNode> *children;                  // Children of class
   Basicness basic_status;                    // `Basic' if class is basic
                                              // `NotBasic' otherwise
   std::vector<method_class*> methods;
   std::vector<method_class*> full_methods;
   std::map<Symbol, int> dispatch_idx_tab;
   std::map<Symbol, Symbol> dispatch_class_tab;
   bool methods_built;
   bool full_methods_built;
   std::vector<attr_class*> attrs;
   std::vector<attr_class*> full_attrs;
   std::map<Symbol, int> attrib_idx_tab;
   bool attrs_built;
   bool full_attrs_built;

public:
   CgenNode(Class_ c,
            Basicness bstatus,
            CgenClassTableP class_table);
   int class_tag;
   int max_tag;

   void add_child(CgenNodeP child);
   List<CgenNode> *get_children() { return children; }
   void set_parentnd(CgenNodeP p);
   CgenNodeP get_parentnd() { return parentnd; }
   int basic() { return (basic_status == Basic); }
   std::vector<CgenNode*> get_inheritance();
   std::vector<method_class*> get_methods();
   std::vector<method_class*> get_full_methods();
   std::vector<attr_class*> get_attrs();
   std::vector<attr_class*> get_full_attribs();
   const std::map<Symbol, int>& get_attrib_idx_tab() const { return attrib_idx_tab; }
   const std::map<Symbol, int>& get_dispatch_idx_tab() const { return dispatch_idx_tab; }
   const std::map<Symbol, Symbol>& get_dispatch_class_tab() const { return dispatch_class_tab; }
   void code_prot_obj(ostream& s);
   void code_init(ostream& s);
};

class BoolConst 
{
 private: 
  int val;
 public:
  BoolConst(int);
  void code_def(ostream&, int boolclasstag);
  void code_ref(ostream&) const;
};
