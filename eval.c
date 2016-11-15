#include "eval.h"

lval* lval_eval_sexpr(lenv* e, lval* v){
  // evaluate children
  for (int i = 0; i < v-> count; i++) {
    v -> cell[i] = lval_eval(e, v -> cell[i]);
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
  if(f -> type != LVAL_FUN) {
    lval_del(f); lval_del(v);
    return lval_err("first element is not a function");
  }

  // calculate
  lval* result = f -> fun(e, v);
  lval_del(f);
  return result;
}

lval* lval_eval(lenv* e, lval *v){
  if(v -> type == LVAL_SYM){
    lval* x = lenv_get(e, v);
    lval_del(v);
    return x;
  }
  // evaluate s expressions
  if (v -> type == LVAL_SEXPR) { return lval_eval_sexpr(e, v); }
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

// macro to check if condition is met, if not delete lval and return error
#define LASSERT(args, cond, err) if(!cond){ lval_del(args); return lval_err(err); }

lval* builtin_head(lenv* e, lval* a){
  // head should have only 1 args
  LASSERT(a, (a -> count == 1), "Function `head` has too many arguments");
  // argument should be a q-epr
  LASSERT(a, (a -> cell[0] -> type == LVAL_QEXPR), "Function `head` passed incorrect type!");
  //  qexpr should not be empty
  LASSERT(a, (a -> cell[0] -> count != 0), "Function `head` passed empty q-epr!");
  // otherwise take 1st child or qexpr
  lval* v = lval_take(a, 0);
  // delete all elements that are not head and return
  while( v -> count > 1){ lval_del(lval_pop(v, 1)); }
  return v;
}

lval* builtin_tail(lenv* e, lval* a){
  // tail should have only 1 args
  LASSERT(a, (a -> count == 1), "Function `tail` has too many arguments");
  // argument should be a q-epr
  LASSERT(a, (a -> cell[0] -> type == LVAL_QEXPR), "Function `tail` passed incorrect type!");
  //  qexpr should not be empty
  LASSERT(a, (a -> cell[0] -> count != 0), "Function `tail` passed empty q-epr!");
  // otherwise take 1st arg
  lval* v = lval_take(a, 0);
  // delete first element and return
  lval_del(lval_pop(v, 0));
  return v;
}

lval* builtin_list(lenv* e, lval* a){
  a -> type = LVAL_QEXPR;
  return a;
}

lval* builtin_eval(lenv* e, lval* a){
  LASSERT(a, (a -> count == 1), "Function `eval` passed too many arguments!");
  LASSERT(a, (a -> cell[0] -> type == LVAL_QEXPR), "Function `eval` passed incorrect type!");

  lval* x = lval_take(a, 0);
  x -> type = LVAL_SEXPR;
  return lval_eval(e, x);
}

lval* lval_join(lval* x, lval* y){
  // push elements of y into x
  while(y -> count){
    x = lval_add(x, lval_pop(y, 0));
  }
  //delete y and return x
  lval_del(y);
  return x;
}

lval* builtin_join(lenv* e, lval* a){
  for(int i = 0; i < a -> count; i++){
    LASSERT(a, (a-> cell[i] -> type == LVAL_QEXPR), "Function `join` passed incorrtect type");
  }

  lval* x = lval_pop(a, 0);

  while(a -> count){
    x = lval_join(x, lval_pop(a, 0));
  }
  lval_del(a);
  return x;
}

lval* builtin_op(lenv* e, lval* a, char* op){
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
lval* builtin_add(lenv* e, lval* a) {
  return builtin_op(e, a, "+");
}

lval* builtin_sub(lenv* e, lval* a) {
  return builtin_op(e, a, "-");
}

lval* builtin_mul(lenv* e, lval* a) {
  return builtin_op(e, a, "*");
}

lval* builtin_div(lenv* e, lval* a) {
  return builtin_op(e, a, "/");
}


void lenv_add_builtins(lenv* e){
   /* List Functions */
  lenv_add_builtin(e, "list", builtin_list);
  lenv_add_builtin(e, "head", builtin_head);
  lenv_add_builtin(e, "tail", builtin_tail);
  lenv_add_builtin(e, "eval", builtin_eval);
  lenv_add_builtin(e, "join", builtin_join);

  /* Mathematical Functions */
  lenv_add_builtin(e, "+", builtin_add);
  lenv_add_builtin(e, "-", builtin_sub);
  lenv_add_builtin(e, "*", builtin_mul);
  lenv_add_builtin(e, "/", builtin_div);
}