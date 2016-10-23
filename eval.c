#include "eval.h"

lval* lval_eval_sexpr(lval* v){
  // evaluate children
  for (int i = 0; i < v-> count; i++) {
    v -> cell[i] = lval_eval(v -> cell[i]);
  }

  // error checking
  for (int i = 0; i < v-> count; i++) {
    if(v -> cell[i] -> type == LVAL_ERR){ return lval_take(v, i); }
  }

  // empty expression
  if(v -> count == 0){
    return v;
  }

  // single expression
  if(v -> count == 1){ return lval_take(v, 0); }

  // ensure first element is symbol
  lval* f = lval_pop(v, 0);
  if(f -> type != LVAL_SYM) {
    lval_del(f); lval_del(v);
    return lval_err("Symbolic expression does not start with symbol!");
  }

  // calculate
  lval* result = calculate(v, f -> sym);
  lval_del(f);
  return result;
}

lval* lval_eval(lval *v){
  // evaluate s expressions
  if (v -> type == LVAL_SEXPR) { return lval_eval_sexpr(v); }
  return v;
}

lval* lval_pop(lval* v, int i){
  // get item at i
  lval* x = v -> cell[i];
  
  // shift memory after item i to i
  memmove(&v -> cell[i], &v -> cell[i + 1], sizeof(lval*) * (v -> count-i-1));
  
  //decrement count
  v -> count--;

  // reallocate memory
  v -> cell = realloc(v->cell, sizeof(lval*) * v->count);
  return x;
}

lval* lval_take(lval* v, int i){
  lval* x = lval_pop(v, i);
  lval_del(v);
  return x;
}

lval* calculate(lval* a, char* op){
  // all args should be numbers
  for (int i = 0; i < a -> count; i++){
    if(a -> cell[i] -> type != LVAL_NUM){
      lval_del(a);
      return lval_err("Cannot operate on non-number");
    }
  }

  //pop first element
  lval* x = lval_pop(a, 0);

  //if no arguments and op is "-" negate value
  if((strcmp(op, "-") == 0) && (a -> count == 0)){
    x -> num = - x -> num;
  }

  while(a -> count > 0){
    lval* y = lval_pop(a, 0);
    if(strcmp(op, "+") == 0){ x -> num += y -> num; }
    if(strcmp(op, "-") == 0){ x -> num -= y -> num; }
    if(strcmp(op, "*") == 0){ x -> num *= y -> num; }
    if(strcmp(op, "/") == 0){
      if(y -> num == 0){
        lval_del(a);
        lval_del(y);
        return lval_err("divisions by zero");
      }
      x -> num /= y -> num;
    }
    lval_del(y);
  }
  lval_del(a);
  return x;
}