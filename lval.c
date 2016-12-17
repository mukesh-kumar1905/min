#include <stdio.h>
#include <string.h>
#include "lval.h"

char* ltype_name(int t) {
  switch(t) {
    case LVAL_FUN: return "Function";
    case LVAL_NUM: return "Number";
    case LVAL_ERR: return "Error";
    case LVAL_SYM: return "Symbol";
    case LVAL_SEXPR: return "S-Expression";
    case LVAL_QEXPR: return "Q-Expression";
    default: return "Unknown";
  }
}

// Construct a pointer to new number lval
lval* lval_num(long x){
  lval* v = malloc(sizeof(lval));
  v -> type = LVAL_NUM;
  v -> num = x;
  return v;
}

// Construct a pointer to new error lval
lval* lval_err(char* fmt, ...){
  lval* v = malloc(sizeof(lval));
  v -> type = LVAL_ERR;

  /* Create a va list and initialize it */
  va_list va;
  va_start(va, fmt);

  // allocate 512 bytes for error
  v -> err = malloc(512);

  // printf the error string with a maximum of 511 characters
  vsnprintf(v->err, 511, fmt, va);

  // Reallocate to number of bytes actually used
  v->err = realloc(v->err, strlen(v->err)+1);

  va_end(va);

  return v;
}

// Construct a pointer to new symbol lval
lval* lval_sym(char* s){
  lval* v = malloc(sizeof(lval));
  v -> type = LVAL_SYM;
  v -> sym = malloc(strlen(s) + 1);
  strcpy(v->sym, s);
  return v;
}

// Construct a pointer to new s-expression lval
lval* lval_sexpr(){
  lval* v = malloc(sizeof(lval));
  v -> type = LVAL_SEXPR;
  v -> count = 0;
  v -> cell = NULL;
  return v;
}

// Construct a pointer to new q-expression lval
lval* lval_qexpr(){
  lval* v = malloc(sizeof(lval));
  v -> type = LVAL_QEXPR;
  v -> count = 0;
  v -> cell = NULL;
  return v;
}

// add lval to s-expression of another lval
lval* lval_add(lval* v, lval *x){
  v -> count++;
  v -> cell = realloc(v -> cell, sizeof(lval*) * v -> count);
  v -> cell[v -> count - 1] = x;
  return v;
}

lval* lval_fun(lcalculate func){
  lval* v = malloc(sizeof(lval));
  v -> type = LVAL_FUN;
  v -> builtin = func;
  return v;
}

lval* lval_read_num(mpc_ast_t* t){
  errno = 0;
  long x = strtol(t -> contents, NULL, 10);
  return errno != ERANGE ? lval_num(x) : lval_err("invalid number");
}

lval* lval_lambda(lval* formals, lval* body){
  lval* v = malloc(sizeof(lval));
  v -> type = LVAL_FUN;

  // set builtin to null
  v -> builtin = NULL;

  v -> env = lenv_new();
  // set formals and body
  v -> formals = formals;
  v -> body = body;
  return v;
}

lval* lval_read(mpc_ast_t* t){
  // if symbol or number convert for that type and return
  if(strstr(t->tag, "number")){ return lval_read_num(t); }
  if(strstr(t->tag, "symbol")){ return lval_sym(t -> contents); }

  // if root or sexpr create empty list
  lval* x = NULL;
  if(strcmp(t -> tag, ">") == 0){ x = lval_sexpr(); }
  if(strcmp(t -> tag, "sexpr")){ x = lval_sexpr(); }
  if(strstr(t -> tag, "qexpr")){ x = lval_qexpr(); }

  // fill the list with valid expressions within
  for (int i = 0; i < t -> children_num; i++)
  {
    if (strcmp(t->children[i]->contents, "(") == 0) { continue; }
    if (strcmp(t->children[i]->contents, ")") == 0) { continue; }
    if (strcmp(t->children[i]->contents, "}") == 0) { continue; }
    if (strcmp(t->children[i]->contents, "{") == 0) { continue; }
    if (strcmp(t->children[i]->tag,  "regex") == 0) { continue; }
    x = lval_add(x, lval_read(t -> children[i]));
  }
  return x;
}

int lval_eq(lval* x, lval* y){
  // different types are unequal
  if(x -> type != y -> type){ return 0; }
  switch(x -> type){
    // compare numbers and strings directly
    case LVAL_NUM : return (x -> num == y -> num);
    case LVAL_ERR : return (strcmp(x -> err, y -> err) == 0);
    case LVAL_SYM : return (strcmp(x -> sym, y -> sym) == 0);
    // if builtin compare, else compare body and formals
    case LVAL_FUN:
      if(x -> builtin || y -> builtin){
        return x -> builtin == y -> builtin;
      } else {
        return lval_eq(x -> formals, y -> formals) && lval_eq(x -> body, y -> body);
      }
    // if list compare each element
    case LVAL_QEXPR:
    case LVAL_SEXPR:
      if(x -> count != y -> count){ return 0; }
      for(int i = 0; i < x -> count; i++){
        // if any cell unequal , lists unequal
        if(!lval_eq(x -> cell[i], y -> cell[i])){ return 0; }
      }
      // if all equal list equal
      return 1;
    break;
  }
  return 0;
}

//return a copy of lval
lval* lval_copy(lval* v){
  lval* x = malloc(sizeof(lval));
  x -> type = v -> type;

  switch(v -> type){
    // copy number & funcs directly
    case LVAL_NUM: x -> num = v -> num; break;
    case LVAL_FUN:
    if(v -> builtin){
      x -> builtin = v -> builtin;
    } else {
      x -> builtin = NULL;
      x -> env = lenv_copy(v -> env);
      x -> formals = lval_copy(v -> formals);
      x -> body = lval_copy(v -> body);
    }
    break;

    //allocate and copy to new string
    case LVAL_SYM:
      x -> sym = malloc(strlen(v -> sym) + 1);
      strcpy(x -> sym, v -> sym);
      break;
    case LVAL_ERR:
      x -> err = malloc(strlen(x -> err) + 1);
      strcpy(x -> err, v -> err);
      break;
    // copy lists by copying sub-epr list
    case LVAL_SEXPR:
    case LVAL_QEXPR:
      x -> count = v -> count;
      x -> cell = malloc(sizeof(lval*) * (x -> count));
      for (int i = 0; i < x -> count; i++)
      {
        x -> cell[i] = lval_copy(v -> cell[i]);
      }
      break;
  }
  return x;
}

// free up memory from lval
void lval_del(lval* v){
  switch (v -> type){
    // do nothing for primitive data and function pointers
    case LVAL_NUM: break;
    case LVAL_FUN:
      if(!v -> builtin){
        lenv_del(v -> env);
        lval_del(v -> formals);
        lval_del(v -> body);
      }
    break;

    // free up strings
    case LVAL_ERR: free(v->err); break;
    case LVAL_SYM: free(v->sym); break;

    // delete all items inside s-expr or q-expr
    case LVAL_QEXPR:
    case LVAL_SEXPR:
      for (int i = 0; i < v -> count; i++)
      {
        lval_del(v -> cell[i]);
      }
      // free the pointer container
      free(v -> cell);
      break;
  }
  free(v);
}

// get new execution environment
lenv* lenv_new(void){
  lenv* e = malloc(sizeof(lenv));
  e -> par = NULL;
  e -> count = 0;
  e -> syms = NULL;
  e -> vals = NULL;
  return e;
}


void lenv_del(lenv* e){
  for (int i = 0; i < e -> count; i++)
  {
    free(e -> syms[i]);
    lval_del(e -> vals[i]);
  }
  free(e -> syms);
  free(e -> vals);
  free(e);
}


lenv* lenv_copy(lenv* e){
  lenv* n = malloc(sizeof(lenv));
  n -> par = e -> par;
  n -> count = e -> count;
  n -> syms = malloc(sizeof(char*) * n->count);
  n -> vals = malloc(sizeof(lval*) * n -> count);
  for (int i = 0; i < e -> count; i++) {
    n -> syms[i] = malloc(strlen(e -> syms[i]) + 1);
    strcpy(n -> syms[i], e -> syms[i]);
    n -> vals[i] = lval_copy(e -> vals[i]);
  }
  return n;
}

// get symbol from environment
lval* lenv_get(lenv* e, lval* k){
  for (int i = 0; i < e -> count; ++i)
  {
    if(strcmp(e -> syms[i], k -> sym) == 0){
      return lval_copy(e -> vals[i]);
    }
  }
  // if no symbol check in parent
  if (e -> par){
    return lenv_get(e -> par, k);
  }
  return lval_err("unbound symbol '%s'", k -> sym);
}

// put value in environment
void lenv_put(lenv* e, lval* k, lval* v){
  for (int i = 0; i < e -> count; ++i)
  {
    if(strcmp(e -> syms[i], k -> sym) == 0){
      lval_del(e -> vals[i]);
      e -> vals[i] = lval_copy(v);
      return;
    }
  }

  e -> count++;
  e -> vals = realloc(e -> vals, (e -> count) * sizeof(lval*));
  e -> syms = realloc(e -> syms, (e -> count) * sizeof(char*));

  e -> vals[e -> count - 1] = lval_copy(v);
  e -> syms[e -> count - 1] = malloc(strlen(k -> sym) + 1);
  strcpy(e -> syms[e -> count -1], k -> sym);
}

// add to global environment
void lenv_def(lenv* e, lval* k, lval* v){
  // get to global environment
  while(e -> par){ e = e -> par; }
  lenv_put(e, k, v);
}

void lenv_add_builtin(lenv* e, char* name, lcalculate func){
  lval* k = lval_sym(name);
  lval* v = lval_fun(func);
  lenv_put(e, k, v);
  lval_del(k);
  lval_del(v);
}

void lval_expr_print(lval* v, char open, char close){
  putchar(open);
  for (int i = 0; i < v -> count; i++)
  {
    // print value contained within
    lval_print(v -> cell[i]);

    // no trailing space for last element
    if (i != (v->count-1)) {
      putchar(' ');
    }
  }
  putchar(close);
}

// print lval
void lval_print(lval* v){
  switch (v -> type){
    case LVAL_NUM: printf("%li", v -> num); break;
    case LVAL_FUN:
      if(v -> builtin){
        printf("<builtin>");  
      } else {
        printf("(\\ ");
        lval_print(v -> formals);
        putchar(' ');
        lval_print(v -> body);
        putchar(')');
      }
      break;
    case LVAL_ERR: printf("Error: %s", v -> err); break;
    case LVAL_SYM:   printf("%s", v->sym); break;
    case LVAL_SEXPR: lval_expr_print(v, '(', ')'); break;
    case LVAL_QEXPR: lval_expr_print(v, '{', '}'); break;
  }
}

// print lval followed by newline
void lval_println(lval* v) { lval_print(v); putchar('\n'); }