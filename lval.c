#include <stdio.h>
#include "lval.h"

// Construct a pointer to new number lval
lval* lval_num(long x){
  lval* v = malloc(sizeof(lval));
  v -> type = LVAL_NUM;
  v -> num = x;
  return v;
}

// Construct a pointer to new error lval
lval* lval_err(char* m){
  lval* v = malloc(sizeof(lval));
  v -> type = LVAL_ERR;
  v -> err = malloc(strlen(m) + 1);
  strcpy(v->err, m);
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

lval* lval_add(lval* v, lval *x){
  v -> count++;
  v -> cell = realloc(v -> cell, sizeof(lval*) * v -> count);
  v -> cell[v -> count - 1] = x;
  return v;
}

lval* lval_read_num(mpc_ast_t* t){
  errno = 0;
  long x = strtol(t -> contents, NULL, 10);
  return errno != ERANGE ? lval_num(x) : lval_err("invalid number");
}

lval* lval_read(mpc_ast_t* t){
  // if symbol or number convert for that type and return
  if(strstr(t->tag, "number")){ return lval_read_num(t); }
  if(strstr(t->tag, "symbol")){ return lval_sym(t -> contents); }

  // if root or sexpr create empty list
  lval* x = NULL;
  if(strcmp(t -> tag, ">") == 0){ x = lval_sexpr(); }
  if(strcmp(t -> tag, "sexpr")){ x = lval_sexpr(); }

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
// free up memory from lval
void lval_del(lval* v){
  switch (v -> type){
    // do nothing for primitive data
    case LVAL_NUM: break;

    // free up strings
    case LVAL_ERR: free(v->err); break;
    case LVAL_SYM: free(v->err); break;

    // delete all items inside s-expr
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
    case LVAL_ERR: printf("Error: %s", v -> err); break;
    case LVAL_SYM:   printf("%s", v->sym); break;
    case LVAL_SEXPR: lval_expr_print(v, '(', ')'); break;
  }
}

// print lval followed by newline
void lval_println(lval* v) { lval_print(v); putchar('\n'); }