

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <set>
#include "semant.h"
#include "utilities.h"


extern int semant_debug;
extern char *curr_filename;

//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////
static Symbol 
    arg,
    arg2,
    Bool,
    concat,
    cool_abort,
    copy,
    Int,
    in_int,
    in_string,
    IO,
    length,
    Main,
    main_meth,
    No_class,
    No_type,
    Object,
    out_int,
    out_string,
    prim_slot,
    self,
    SELF_TYPE,
    Str,
    str_field,
    substr,
    type_name,
    val;
//
// Initializing the predefined symbols.
//
static void initialize_constants(void)
{
    arg         = idtable.add_string("arg");
    arg2        = idtable.add_string("arg2");
    Bool        = idtable.add_string("Bool");
    concat      = idtable.add_string("concat");
    cool_abort  = idtable.add_string("abort");
    copy        = idtable.add_string("copy");
    Int         = idtable.add_string("Int");
    in_int      = idtable.add_string("in_int");
    in_string   = idtable.add_string("in_string");
    IO          = idtable.add_string("IO");
    length      = idtable.add_string("length");
    Main        = idtable.add_string("Main");
    main_meth   = idtable.add_string("main");
    //   _no_class is a symbol that can't be the name of any 
    //   user-defined class.
    No_class    = idtable.add_string("_no_class");
    No_type     = idtable.add_string("_no_type");
    Object      = idtable.add_string("Object");
    out_int     = idtable.add_string("out_int");
    out_string  = idtable.add_string("out_string");
    prim_slot   = idtable.add_string("_prim_slot");
    self        = idtable.add_string("self");
    SELF_TYPE   = idtable.add_string("SELF_TYPE");
    Str         = idtable.add_string("String");
    str_field   = idtable.add_string("_str_field");
    substr      = idtable.add_string("substr");
    type_name   = idtable.add_string("type_name");
    val         = idtable.add_string("_val");
}



ClassTable::ClassTable(Classes classes) : semant_errors_(0) , error_stream_(cerr) {
    class_table_ = new SymbolTable<Symbol, Class_>;
    class_table_->enterscope();
    install_basic_classes();
    build_inheritance_graph(classes);
    check_inheritance(classes);
    check_features(classes);
}

void ClassTable::install_basic_classes() {

    // The tree package uses these globals to annotate the classes built below.
   // curr_lineno  = 0;
    Symbol filename = stringtable.add_string("<basic class>");
    
    // The following demonstrates how to create dummy parse trees to
    // refer to basic Cool classes.  There's no need for method
    // bodies -- these are already built into the runtime system.
    
    // IMPORTANT: The results of the following expressions are
    // stored in local variables.  You will want to do something
    // with those variables at the end of this method to make this
    // code meaningful.

    // 
    // The Object class has no parent class. Its methods are
    //        abort() : Object    aborts the program
    //        type_name() : Str   returns a string representation of class name
    //        copy() : SELF_TYPE  returns a copy of the object
    //
    // There is no need for method bodies in the basic classes---these
    // are already built in to the runtime system.

    Object_class_ =
	class_(Object, 
	       No_class,
	       append_Features(
			       append_Features(
					       single_Features(method(cool_abort, nil_Formals(), Object, no_expr())),
					       single_Features(method(type_name, nil_Formals(), Str, no_expr()))),
			       single_Features(method(copy, nil_Formals(), SELF_TYPE, no_expr()))),
	       filename);
    class_table_->addid(Object, &Object_class_);

    // 
    // The IO class inherits from Object. Its methods are
    //        out_string(Str) : SELF_TYPE       writes a string to the output
    //        out_int(Int) : SELF_TYPE            "    an int    "  "     "
    //        in_string() : Str                 reads a string from the input
    //        in_int() : Int                      "   an int     "  "     "
    //
    IO_class_ = 
	class_(IO, 
	       Object,
	       append_Features(
			       append_Features(
					       append_Features(
							       single_Features(method(out_string, single_Formals(formal(arg, Str)),
										      SELF_TYPE, no_expr())),
							       single_Features(method(out_int, single_Formals(formal(arg, Int)),
										      SELF_TYPE, no_expr()))),
					       single_Features(method(in_string, nil_Formals(), Str, no_expr()))),
			       single_Features(method(in_int, nil_Formals(), Int, no_expr()))),
	       filename);  
    class_table_->addid(IO, &IO_class_);

    //
    // The Int class has no methods and only a single attribute, the
    // "val" for the integer. 
    //
    Int_class_ =
	class_(Int, 
	       Object,
	       single_Features(attr(val, prim_slot, no_expr())),
	       filename);
    class_table_->addid(Int, &Int_class_);

    //
    // Bool also has only the "val" slot.
    //
    Bool_class_ =
	class_(Bool, Object, single_Features(attr(val, prim_slot, no_expr())),filename);
    class_table_->addid(Bool, &Bool_class_);

    //
    // The class Str has a number of slots and operations:
    //       val                                  the length of the string
    //       str_field                            the string itself
    //       length() : Int                       returns length of the string
    //       concat(arg: Str) : Str               performs string concatenation
    //       substr(arg: Int, arg2: Int): Str     substring selection
    //       
    Str_class_ =
	class_(Str, 
	       Object,
	       append_Features(
			       append_Features(
					       append_Features(
							       append_Features(
									       single_Features(attr(val, Int, no_expr())),
									       single_Features(attr(str_field, prim_slot, no_expr()))),
							       single_Features(method(length, nil_Formals(), Int, no_expr()))),
					       single_Features(method(concat, 
								      single_Formals(formal(arg, Str)),
								      Str, 
								      no_expr()))),
			       single_Features(method(substr, 
						      append_Formals(single_Formals(formal(arg, Int)), 
								     single_Formals(formal(arg2, Int))),
						      Str, 
						      no_expr()))),
	       filename);
    class_table_->addid(Str, &Str_class_);
}

void ClassTable::build_inheritance_graph(Classes classes) {
    int class_num = classes->len();
    for(int i = 0; i < class_num; i++) {
        Class_ c = classes->nth(i);
        Symbol name = c->get_name();
        if (name == SELF_TYPE) {
            semant_error(c) << "Class name cannot be SELF_TYPE." << endl;
            continue;
        }
        if (class_table_->probe(name) != NULL) {
            semant_error(c) << "Redefinition of class " << name << endl;
            continue;
        }
        Class_ *class_ptr = new Class_(c);
        class_table_->addid(name, class_ptr);
    }
}

void ClassTable::check_inheritance(Classes classes) {
    int class_num = classes->len();
    for(int i = 0; i < class_num; i++) {
        Class_ c = classes->nth(i);
        check_class_inheritance(c);
    }
}

void ClassTable::check_class_inheritance(Class_ c) {
    Symbol name = c->get_name();
    Symbol parent = c->get_parent();

    if (parent == SELF_TYPE || parent == Int || parent == Bool || parent == Str) {
        semant_error(c) << "Class " << name << " cannot inherit class "
                        << parent << "." << endl;
        return;
    }

    if (lookup_class(parent) == NULL) {
        semant_error(c) << "Class " << name << " inherits from an undefined class "
                        << parent << "." << endl;
        return;
    }

    std::set<Symbol> seen;
    Symbol cur = name;
    while (cur != Object && cur != No_class) {
        if (seen.count(cur) != 0) {
            semant_error(c) << "Class " << name
                            << ", or an ancestor of " << name
                            << ", is involved in an inheritance cycle."
                            << endl;
            break;
        }
        seen.insert(cur);
        Class_ cur_class = lookup_class(cur);
        if (cur_class == NULL) {
            break;
        }
        cur = cur_class->get_parent();
    }
}

Symbol ClassTable::resolve_type(Symbol type, Class_ current_class) {
    if (type == SELF_TYPE) {
        return current_class->get_name();
    }
    return type;
}

bool ClassTable::is_subtype(Symbol child, Symbol parent, Class_ current_class) {
    if (child == No_type) {
        return true;
    }
    if (parent == SELF_TYPE) {
        return child == SELF_TYPE;
    }
    if (child == SELF_TYPE) {
        child = current_class->get_name();
    }
    Symbol cur = child;
    while (cur != No_class) {
        if (cur == parent) {
            return true;
        }
        Class_ cur_class = lookup_class(cur);
        if (cur_class == NULL) {
            break;
        }
        cur = cur_class->get_parent();
    }
    return false;
}

Symbol ClassTable::lub(Symbol a, Symbol b, Class_ current_class) {
    if (a == SELF_TYPE && b == SELF_TYPE) {
        return SELF_TYPE;
    }
    a = resolve_type(a, current_class);
    b = resolve_type(b, current_class);
    if (a == b) {
        return a;
    }
    std::set<Symbol> ancestors;
    Symbol cur = a;
    while (cur != No_class) {
        ancestors.insert(cur);
        Class_ cur_class = lookup_class(cur);
        if (cur_class == NULL) {
            break;
        }
        cur = cur_class->get_parent();
    }
    cur = b;
    while (cur != No_class) {
        if (ancestors.count(cur) != 0) {
            return cur;
        }
        Class_ cur_class = lookup_class(cur);
        if (cur_class == NULL) {
            break;
        }
        cur = cur_class->get_parent();
    }
    return Object;
}

void ClassTable::add_inherited_attrs(Class_ c, SymbolTable<Symbol, Symbol> *env) {
    Symbol parent = c->get_parent();
    if (parent == No_class) {
        return;
    }
    Class_ parent_class = lookup_class(parent);
    if (parent_class == NULL) {
        return;
    }
    add_inherited_attrs(parent_class, env);
    Features features = parent_class->get_features();
    for (int i = features->first(); features->more(i); i = features->next(i)) {
        Feature feature = features->nth(i);
        if (feature->is_attr()) {
            attr_class *attr = static_cast<attr_class*>(feature);
            Symbol name = attr->get_name();
            Symbol type_decl = attr->get_type_decl();
            env->addid(name, new Symbol(type_decl));
        }
    }
}

static attr_class *find_attr_in_class(Class_ c, Symbol name) {
    Features features = c->get_features();
    for (int i = features->first(); features->more(i); i = features->next(i)) {
        Feature feature = features->nth(i);
        if (feature->is_attr() && feature->get_name() == name) {
            return static_cast<attr_class*>(feature);
        }
    }
    return NULL;
}

static attr_class *find_attr_in_ancestors(ClassTable *classtable, Class_ c, Symbol name) {
    Symbol parent = c->get_parent();
    while (parent != No_class) {
        Class_ parent_class = classtable->lookup_class(parent);
        if (parent_class == NULL) {
            break;
        }
        attr_class *found = find_attr_in_class(parent_class, name);
        if (found != NULL) {
            return found;
        }
        parent = parent_class->get_parent();
    }
    return NULL;
}

static method_class *find_method_in_class(Class_ c, Symbol name) {
    Features features = c->get_features();
    for (int i = features->first(); features->more(i); i = features->next(i)) {
        Feature feature = features->nth(i);
        if (feature->is_method() && feature->get_name() == name) {
            return static_cast<method_class*>(feature);
        }
    }
    return NULL;
}

static method_class *find_method_in_ancestors(ClassTable *classtable, Class_ c, Symbol name) {
    Symbol parent = c->get_parent();
    while (parent != No_class) {
        Class_ parent_class = classtable->lookup_class(parent);
        if (parent_class == NULL) {
            break;
        }
        method_class *found = find_method_in_class(parent_class, name);
        if (found != NULL) {
            return found;
        }
        parent = parent_class->get_parent();
    }
    return NULL;
}

static bool same_formal_types(Formals a, Formals b) {
    if (a->len() != b->len()) {
        return false;
    }
    int len = a->len();
    for (int i = 0; i < len; i++) {
        Formal fa = a->nth(i);
        Formal fb = b->nth(i);
        if (fa->get_type_decl() != fb->get_type_decl()) {
            return false;
        }
    }
    return true;
}

method_class *ClassTable::lookup_method(Symbol class_name, Symbol method_name) {
    Symbol cur = class_name;
    while (cur != No_class) {
        Class_ cur_class = lookup_class(cur);
        if (cur_class == NULL) {
            break;
        }
        method_class *method = find_method_in_class(cur_class, method_name);
        if (method != NULL) {
            return method;
        }
        cur = cur_class->get_parent();
    }
    return NULL;
}

Class_ ClassTable::lookup_class(Symbol class_name) {
    Class_ *found = class_table_->lookup(class_name);
    if (found == NULL) {
        return NULL;
    }
    return *found;
}

void ClassTable::check_features(Classes classes) {
    int class_num = classes->len();
    for(int i = 0; i < class_num; i++) {
        Class_ c = classes->nth(i);
        check_class_features(c);
    }
}

void ClassTable::check_class_features(Class_ c) {
    std::set<Symbol> attr_names;
    std::set<Symbol> method_names;
    Features features = c->get_features();
    for (int i = features->first(); features->more(i); i = features->next(i)) {
        Feature feature = features->nth(i);
        if (feature->is_attr()) {
            attr_class *attr = static_cast<attr_class*>(feature);
            Symbol name = attr->get_name();
            Symbol type_decl = attr->get_type_decl();
            if (name == self) {
                semant_error(c) << "'self' cannot be the name of an attribute." << endl;
                continue;
            }
            if (attr_names.count(name) != 0) {
                semant_error(c) << "Attribute " << name
                                << " is multiply defined in class "
                                << c->get_name() << "." << endl;
                continue;
            }
            attr_names.insert(name);
            if (type_decl != SELF_TYPE && lookup_class(type_decl) == NULL) {
                semant_error(c) << "Attribute " << name
                                << " declared with undefined type "
                                << type_decl << "." << endl;
            }
            if (find_attr_in_ancestors(this, c, name) != NULL) {
                semant_error(c) << "Attribute " << name
                                << " is an attribute of an inherited class."
                                << endl;
            }
        } else if (feature->is_method()) {
            method_class *method = static_cast<method_class*>(feature);
            Symbol name = method->get_name();
            if (method_names.count(name) != 0) {
                semant_error(c) << "Method " << name
                                << " is multiply defined in class "
                                << c->get_name() << "." << endl;
                continue;
            }
            method_names.insert(name);
            Symbol return_type = method->get_return_type();
            if (return_type != SELF_TYPE && lookup_class(return_type) == NULL) {
                semant_error(c) << "Method " << name
                                << " declared with undefined return type "
                                << return_type << "." << endl;
            }

            std::set<Symbol> formal_names;
            Formals formals = method->get_formals();
            for (int j = formals->first(); formals->more(j); j = formals->next(j)) {
                Formal formal = formals->nth(j);
                Symbol formal_name = formal->get_name();
                Symbol formal_type = formal->get_type_decl();
                if (formal_name == self) {
                    semant_error(c) << "'self' cannot be the name of a formal." << endl;
                    continue;
                }
                if (formal_names.count(formal_name) != 0) {
                    semant_error(c) << "Formal " << formal_name
                                    << " is multiply defined in method "
                                    << name << "." << endl;
                    continue;
                }
                formal_names.insert(formal_name);
                if (formal_type == SELF_TYPE) {
                    semant_error(c) << "Formal " << formal_name
                                    << " cannot have type SELF_TYPE." << endl;
                } else if (lookup_class(formal_type) == NULL) {
                    semant_error(c) << "Formal " << formal_name
                                    << " declared with undefined type "
                                    << formal_type << "." << endl;
                }
            }

            method_class *parent_method = find_method_in_ancestors(this, c, name);
            if (parent_method != NULL) {
                if (parent_method->get_return_type() != return_type ||
                    !same_formal_types(parent_method->get_formals(),
                                       method->get_formals())) {
                    semant_error(c) << "Method " << name
                                    << " does not match inherited method."
                                    << endl;
                }
            }
        }
    }

    SymbolTable<Symbol, Symbol> object_env;
    object_env.enterscope();
    object_env.addid(self, new Symbol(SELF_TYPE));
    add_inherited_attrs(c, &object_env);

    for (int i = features->first(); features->more(i); i = features->next(i)) {
        Feature feature = features->nth(i);
        if (feature->is_attr()) {
            attr_class *attr = static_cast<attr_class*>(feature);
            Symbol name = attr->get_name();
            Symbol type_decl = attr->get_type_decl();
            if (name == self || object_env.lookup(name) != NULL) {
                continue;
            }
            object_env.addid(name, new Symbol(type_decl));
        }
    }

    for (int i = features->first(); features->more(i); i = features->next(i)) {
        Feature feature = features->nth(i);
        if (feature->is_attr()) {
            attr_class *attr = static_cast<attr_class*>(feature);
            Symbol name = attr->get_name();
            Symbol type_decl = attr->get_type_decl();
            if (name == self) {
                continue;
            }
            if (object_env.lookup(name) == NULL) {
                object_env.addid(name, new Symbol(type_decl));
            }
            Expression init = attr->get_init();
            Symbol init_type = init->type_check(this, c, &object_env);
            if (!is_subtype(init_type, type_decl, c)) {
                semant_error(c) << "Inferred type " << init_type
                                << " of initialization of attribute " << name
                                << " does not conform to declared type "
                                << type_decl << "." << endl;
            }
        } else if (feature->is_method()) {
            method_class *method = static_cast<method_class*>(feature);
            Symbol return_type = method->get_return_type();
            object_env.enterscope();
            Formals formals = method->get_formals();
            for (int j = formals->first(); formals->more(j); j = formals->next(j)) {
                Formal formal = formals->nth(j);
                Symbol formal_name = formal->get_name();
                Symbol formal_type = formal->get_type_decl();
                object_env.addid(formal_name, new Symbol(formal_type));
            }
            Symbol body_type = method->get_expr()->type_check(this, c, &object_env);
            if (return_type == SELF_TYPE) {
                if (body_type != SELF_TYPE) {
                    semant_error(c) << "Inferred return type " << body_type
                                    << " of method " << method->get_name()
                                    << " does not conform to declared return type "
                                    << return_type << "." << endl;
                }
            } else if (!is_subtype(body_type, return_type, c)) {
                semant_error(c) << "Inferred return type " << body_type
                                << " of method " << method->get_name()
                                << " does not conform to declared return type "
                                << return_type << "." << endl;
            }
            object_env.exitscope();
        }
    }
}

static void check_dispatch_arguments(ClassTable *classtable,
                                     Class_ current_class,
                                     SymbolTable<Symbol, Symbol> *object_env,
                                     method_class *method,
                                     Expressions actuals,
                                     Class_ error_class,
                                     tree_node *error_node) {
    Formals formals = method->get_formals();
    if (actuals->len() != formals->len()) {
        classtable->semant_error(error_class->get_filename(), error_node)
            << "Method " << method->get_name()
            << " called with wrong number of arguments." << endl;
        return;
    }
    int len = formals->len();
    for (int i = 0; i < len; i++) {
        Expression actual = actuals->nth(i);
        Formal formal = formals->nth(i);
        Symbol actual_type = actual->type_check(classtable, current_class, object_env);
        Symbol formal_type = formal->get_type_decl();
        if (!classtable->is_subtype(actual_type, formal_type, current_class)) {
            classtable->semant_error(error_class->get_filename(), error_node)
                << "In call of method " << method->get_name()
                << ", type " << actual_type
                << " of parameter " << formal->get_name()
                << " does not conform to declared type "
                << formal_type << "." << endl;
        }
    }
}

Symbol assign_class::type_check(ClassTable *classtable, Class_ current_class,
                                SymbolTable<Symbol, Symbol> *object_env) {
    if (name == self) {
        classtable->semant_error(current_class->get_filename(), this)
            << "Cannot assign to 'self'." << endl;
        return set_type(Object)->get_type();
    }
    Symbol *decl_type = object_env->lookup(name);
    if (decl_type == NULL) {
        classtable->semant_error(current_class->get_filename(), this)
            << "Assignment to undeclared variable " << name << "." << endl;
        return set_type(Object)->get_type();
    }
    Symbol expr_type = expr->type_check(classtable, current_class, object_env);
    if (!classtable->is_subtype(expr_type, *decl_type, current_class)) {
        classtable->semant_error(current_class->get_filename(), this)
            << "Type " << expr_type
            << " of assigned expression does not conform to declared type "
            << *decl_type << " of identifier " << name << "." << endl;
    }
    return set_type(expr_type)->get_type();
}

Symbol static_dispatch_class::type_check(ClassTable *classtable, Class_ current_class,
                                         SymbolTable<Symbol, Symbol> *object_env) {
    Symbol expr_type = expr->type_check(classtable, current_class, object_env);
    if (!classtable->is_subtype(expr_type, type_name, current_class)) {
        classtable->semant_error(current_class->get_filename(), this)
            << "Expression type " << expr_type
            << " does not conform to declared static dispatch type "
            << type_name << "." << endl;
    }
    Symbol dispatch_type = type_name;
    if (dispatch_type == SELF_TYPE) {
        dispatch_type = current_class->get_name();
    }
    method_class *method = classtable->lookup_method(dispatch_type, name);
    if (method == NULL) {
        classtable->semant_error(current_class->get_filename(), this)
            << "Dispatch to undefined method " << name << "." << endl;
        return set_type(Object)->get_type();
    }
    check_dispatch_arguments(classtable, current_class, object_env, method, actual,
                             current_class, this);
    Symbol return_type = method->get_return_type();
    if (return_type == SELF_TYPE) {
        return set_type(type_name)->get_type();
    }
    return set_type(return_type)->get_type();
}

Symbol dispatch_class::type_check(ClassTable *classtable, Class_ current_class,
                                  SymbolTable<Symbol, Symbol> *object_env) {
    Symbol expr_type = expr->type_check(classtable, current_class, object_env);
    Symbol dispatch_type = expr_type;
    if (dispatch_type == SELF_TYPE) {
        dispatch_type = current_class->get_name();
    }
    method_class *method = classtable->lookup_method(dispatch_type, name);
    if (method == NULL) {
        classtable->semant_error(current_class->get_filename(), this)
            << "Dispatch to undefined method " << name << "." << endl;
        return set_type(Object)->get_type();
    }
    check_dispatch_arguments(classtable, current_class, object_env, method, actual,
                             current_class, this);
    Symbol return_type = method->get_return_type();
    if (return_type == SELF_TYPE) {
        return set_type(expr_type)->get_type();
    }
    return set_type(return_type)->get_type();
}

Symbol cond_class::type_check(ClassTable *classtable, Class_ current_class,
                              SymbolTable<Symbol, Symbol> *object_env) {
    Symbol pred_type = pred->type_check(classtable, current_class, object_env);
    if (pred_type != Bool) {
        classtable->semant_error(current_class->get_filename(), this)
            << "Predicate of 'if' does not have type Bool." << endl;
    }
    Symbol then_type = then_exp->type_check(classtable, current_class, object_env);
    Symbol else_type = else_exp->type_check(classtable, current_class, object_env);
    return set_type(classtable->lub(then_type, else_type, current_class))->get_type();
}

Symbol loop_class::type_check(ClassTable *classtable, Class_ current_class,
                              SymbolTable<Symbol, Symbol> *object_env) {
    Symbol pred_type = pred->type_check(classtable, current_class, object_env);
    if (pred_type != Bool) {
        classtable->semant_error(current_class->get_filename(), this)
            << "Loop condition does not have type Bool." << endl;
    }
    body->type_check(classtable, current_class, object_env);
    return set_type(Object)->get_type();
}

Symbol typcase_class::type_check(ClassTable *classtable, Class_ current_class,
                                 SymbolTable<Symbol, Symbol> *object_env) {
    expr->type_check(classtable, current_class, object_env);
    std::set<Symbol> branch_types;
    Symbol result_type = No_type;
    Cases cases_list = cases;
    for (int i = cases_list->first(); cases_list->more(i); i = cases_list->next(i)) {
        Case branch = cases_list->nth(i);
        Symbol branch_type = branch->get_type_decl();
        if (branch_type == SELF_TYPE) {
            classtable->semant_error(current_class->get_filename(), this)
                << "Case branch type cannot be SELF_TYPE." << endl;
            continue;
        }
        if (branch_types.count(branch_type) != 0) {
            classtable->semant_error(current_class->get_filename(), this)
                << "Duplicate branch " << branch_type
                << " in case statement." << endl;
        }
        branch_types.insert(branch_type);
        if (classtable->lookup_class(branch_type) == NULL) {
            classtable->semant_error(current_class->get_filename(), this)
                << "Case branch type " << branch_type
                << " is undefined." << endl;
        }
        object_env->enterscope();
        object_env->addid(branch->get_name(), new Symbol(branch_type));
        Symbol body_type = branch->get_expr()->type_check(classtable, current_class, object_env);
        object_env->exitscope();
        if (result_type == No_type) {
            result_type = body_type;
        } else {
            result_type = classtable->lub(result_type, body_type, current_class);
        }
    }
    return set_type(result_type)->get_type();
}

Symbol block_class::type_check(ClassTable *classtable, Class_ current_class,
                               SymbolTable<Symbol, Symbol> *object_env) {
    Symbol last_type = No_type;
    for (int i = body->first(); body->more(i); i = body->next(i)) {
        Expression expr = body->nth(i);
        last_type = expr->type_check(classtable, current_class, object_env);
    }
    return set_type(last_type)->get_type();
}

Symbol let_class::type_check(ClassTable *classtable, Class_ current_class,
                             SymbolTable<Symbol, Symbol> *object_env) {
    if (identifier == self) {
        classtable->semant_error(current_class->get_filename(), this)
            << "'self' cannot be bound in a 'let' expression." << endl;
    }
    if (type_decl == SELF_TYPE) {
        classtable->semant_error(current_class->get_filename(), this)
            << "'SELF_TYPE' cannot be the declared type of a 'let' identifier."
            << endl;
    } else if (classtable->lookup_class(type_decl) == NULL) {
        classtable->semant_error(current_class->get_filename(), this)
            << "Let variable " << identifier
            << " declared with undefined type " << type_decl << "." << endl;
    }
    Symbol init_type = init->type_check(classtable, current_class, object_env);
    if (!classtable->is_subtype(init_type, type_decl, current_class)) {
        classtable->semant_error(current_class->get_filename(), this)
            << "Inferred type " << init_type
            << " of initialization of " << identifier
            << " does not conform to declared type "
            << type_decl << "." << endl;
    }
    object_env->enterscope();
    object_env->addid(identifier, new Symbol(type_decl));
    Symbol body_type = body->type_check(classtable, current_class, object_env);
    object_env->exitscope();
    return set_type(body_type)->get_type();
}

static Symbol arithmetic_check(Expression e1, Expression e2,
                               ClassTable *classtable,
                               Class_ current_class,
                               SymbolTable<Symbol, Symbol> *object_env,
                               tree_node *error_node,
                               const char *op_name) {
    Symbol t1 = e1->type_check(classtable, current_class, object_env);
    Symbol t2 = e2->type_check(classtable, current_class, object_env);
    if (t1 != Int || t2 != Int) {
        classtable->semant_error(current_class->get_filename(), error_node)
            << "Non-Int arguments: " << op_name << "." << endl;
    }
    return Int;
}

Symbol plus_class::type_check(ClassTable *classtable, Class_ current_class,
                              SymbolTable<Symbol, Symbol> *object_env) {
    return set_type(arithmetic_check(e1, e2, classtable, current_class,
                                     object_env, this, "+"))->get_type();
}

Symbol sub_class::type_check(ClassTable *classtable, Class_ current_class,
                             SymbolTable<Symbol, Symbol> *object_env) {
    return set_type(arithmetic_check(e1, e2, classtable, current_class,
                                     object_env, this, "-"))->get_type();
}

Symbol mul_class::type_check(ClassTable *classtable, Class_ current_class,
                             SymbolTable<Symbol, Symbol> *object_env) {
    return set_type(arithmetic_check(e1, e2, classtable, current_class,
                                     object_env, this, "*"))->get_type();
}

Symbol divide_class::type_check(ClassTable *classtable, Class_ current_class,
                                SymbolTable<Symbol, Symbol> *object_env) {
    return set_type(arithmetic_check(e1, e2, classtable, current_class,
                                     object_env, this, "/"))->get_type();
}

Symbol neg_class::type_check(ClassTable *classtable, Class_ current_class,
                             SymbolTable<Symbol, Symbol> *object_env) {
    Symbol t1 = e1->type_check(classtable, current_class, object_env);
    if (t1 != Int) {
        classtable->semant_error(current_class->get_filename(), this)
            << "Argument of '~' has type " << t1
            << " instead of Int." << endl;
    }
    return set_type(Int)->get_type();
}

Symbol lt_class::type_check(ClassTable *classtable, Class_ current_class,
                            SymbolTable<Symbol, Symbol> *object_env) {
    Symbol t1 = e1->type_check(classtable, current_class, object_env);
    Symbol t2 = e2->type_check(classtable, current_class, object_env);
    if (t1 != Int || t2 != Int) {
        classtable->semant_error(current_class->get_filename(), this)
            << "Non-Int arguments: <." << endl;
    }
    return set_type(Bool)->get_type();
}

Symbol leq_class::type_check(ClassTable *classtable, Class_ current_class,
                             SymbolTable<Symbol, Symbol> *object_env) {
    Symbol t1 = e1->type_check(classtable, current_class, object_env);
    Symbol t2 = e2->type_check(classtable, current_class, object_env);
    if (t1 != Int || t2 != Int) {
        classtable->semant_error(current_class->get_filename(), this)
            << "Non-Int arguments: <=." << endl;
    }
    return set_type(Bool)->get_type();
}

Symbol eq_class::type_check(ClassTable *classtable, Class_ current_class,
                            SymbolTable<Symbol, Symbol> *object_env) {
    Symbol t1 = e1->type_check(classtable, current_class, object_env);
    Symbol t2 = e2->type_check(classtable, current_class, object_env);
    if ((t1 == Int || t1 == Bool || t1 == Str ||
         t2 == Int || t2 == Bool || t2 == Str) && t1 != t2) {
        classtable->semant_error(current_class->get_filename(), this)
            << "Illegal comparison with a basic type." << endl;
    }
    return set_type(Bool)->get_type();
}

Symbol comp_class::type_check(ClassTable *classtable, Class_ current_class,
                              SymbolTable<Symbol, Symbol> *object_env) {
    Symbol t1 = e1->type_check(classtable, current_class, object_env);
    if (t1 != Bool) {
        classtable->semant_error(current_class->get_filename(), this)
            << "Argument of 'not' has type " << t1
            << " instead of Bool." << endl;
    }
    return set_type(Bool)->get_type();
}

Symbol int_const_class::type_check(ClassTable *, Class_, SymbolTable<Symbol, Symbol> *) {
    return set_type(Int)->get_type();
}

Symbol bool_const_class::type_check(ClassTable *, Class_, SymbolTable<Symbol, Symbol> *) {
    return set_type(Bool)->get_type();
}

Symbol string_const_class::type_check(ClassTable *, Class_, SymbolTable<Symbol, Symbol> *) {
    return set_type(Str)->get_type();
}

Symbol new__class::type_check(ClassTable *classtable, Class_ current_class,
                              SymbolTable<Symbol, Symbol> *) {
    if (type_name == SELF_TYPE) {
        return set_type(SELF_TYPE)->get_type();
    }
    if (classtable->lookup_class(type_name) == NULL) {
        classtable->semant_error(current_class->get_filename(), this)
            << "'new' used with undefined class " << type_name << "." << endl;
        return set_type(Object)->get_type();
    }
    return set_type(type_name)->get_type();
}

Symbol isvoid_class::type_check(ClassTable *classtable, Class_ current_class,
                                SymbolTable<Symbol, Symbol> *object_env) {
    e1->type_check(classtable, current_class, object_env);
    return set_type(Bool)->get_type();
}

Symbol no_expr_class::type_check(ClassTable *, Class_, SymbolTable<Symbol, Symbol> *) {
    return set_type(No_type)->get_type();
}

Symbol object_class::type_check(ClassTable *classtable, Class_ current_class,
                                SymbolTable<Symbol, Symbol> *object_env) {
    if (name == self) {
        return set_type(SELF_TYPE)->get_type();
    }
    Symbol *type = object_env->lookup(name);
    if (type == NULL) {
        classtable->semant_error(current_class->get_filename(), this)
            << "Undeclared identifier " << name << "." << endl;
        return set_type(Object)->get_type();
    }
    return set_type(*type)->get_type();
}


////////////////////////////////////////////////////////////////////
//
// semant_error is an overloaded function for reporting errors
// during semantic analysis.  There are three versions:
//
//    ostream& ClassTable::semant_error()                
//
//    ostream& ClassTable::semant_error(Class_ c)
//       print line number and filename for `c'
//
//    ostream& ClassTable::semant_error(Symbol filename, tree_node *t)  
//       print a line number and filename
//
///////////////////////////////////////////////////////////////////

ostream& ClassTable::semant_error(Class_ c)
{                                                             
    return semant_error(c->get_filename(),c);
}    

ostream& ClassTable::semant_error(Symbol filename, tree_node *t)
{
    error_stream_ << filename << ":" << t->get_line_number() << ": ";
    return semant_error();
}

ostream& ClassTable::semant_error()                  
{                                                 
    semant_errors_++;                            
    return error_stream_;
} 



/*   This is the entry point to the semantic checker.

     Your checker should do the following two things:

     1) Check that the program is semantically correct
     2) Decorate the abstract syntax tree with type information
        by setting the `type' field in each Expression node.
        (see `tree.h')

     You are free to first do 1), make sure you catch all semantic
     errors. Part 2) can be done in a second stage, when you want
     to build mycoolc.
 */
void program_class::semant()
{
    initialize_constants();

    /* ClassTable constructor may do some semantic analysis */
    ClassTable *classtable = new ClassTable(classes);

    if (classtable->errors()) {
	    cerr << "Compilation halted due to static semantic errors." << endl;
	    exit(1);
    }
}
