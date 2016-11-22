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
    return lval_err("S expr starts with incorrect type. Got %s, Expected %s", ltype_name(f -> type), ltype_name(LVAL_FUN));
    lval_del(f); lval_del(v);
  }

  // calculate
  lval* result = lval_call(e, f, v);
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

// calling a function (builtin or user defined)
lval* lval_call(lenv* e, lval* f, lval* a){
  // if builtin call func
  if(f -> builtin){ return f -> builtin(e, a); }

  // record number of args
  int given = a -> count;
  int total = f -> formals -> count;

  while(a -> count){
    if(f -> formals -> count == 0){
      lval_del(a);
      return lval_err("Function passed too many args. Got %i, Expected %i", given, total);
    }

    // pop symbol from formals
    lval* sym = lval_pop(f -> formals, 0);

    // special case of &
    if(strcmp(sym -> sym, "&") == 0){
      // & has to be followed by another symbol
      if(f -> formals -> count != 1){
        lval_del(a);
        return lval_err("Function format invalid. & should be followed by a single symbol");
      }
      lval* nsym = lval_pop(f -> formals, 0);
      lenv_put(f -> env, nsym, builtin_list(e, a));
      lval_del(sym);
      lval_del(nsym);
      break;
    }

    // pop the next arg from the list
    lval* val = lval_pop(a, 0);

    lenv_put(f -> env, sym, val);

    // clear up symbol and value
    lval_del(sym);
    lval_del(val);
  }

  lval_del(a);

  // if `&` remains in formal argument list, but no args are passed to func
  if(f -> formals -> count > 0 && strcmp(f -> formals -> cell[0] -> sym, "&")){
    // check for valid & formal args
    if(f -> formals -> count != 2){
      return lval_err("Function format invalid. & should be followed by a single symbol");
    }

    // remove and
    lval_del(lval_pop(f -> formals, 0));
    lval* sym = lval_pop(f -> formals, 0);
    lval* val = lval_qexpr();

    // bind to env and delete
    lenv_put(f -> env, sym, val);
    lval_del(sym);
    lval_del(val);
  }

  if(f -> formals -> count == 0){
    // set parent env
    f -> env -> par = e;

    // eval and return
    return builtin_eval(f -> env, lval_add(lval_sexpr(), lval_copy(f -> body)));
  } else {
    // return copy of new func
    return lval_copy(f);
  }

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

lval* builtin_def(lenv* e, lval* a){
  return builtin_var(e, a, "def");
}

lval* builtin_put(lenv* e, lval* a){
  return builtin_var(e, a, "=");
}

lval* builtin_var(lenv* e, lval* a, char* func){
  LASSERT_TYPE(func, a, 0, LVAL_QEXPR);

  // first argument to symbol list, ie, name of variables
  lval* syms = a -> cell[0];

  // all names should be symbols
  for (int i = 0; i < syms -> count; ++i)
  {
    LASSERT(a, (syms -> cell[i] -> type == LVAL_SYM), "Function %s cannot define non symbol. Got %s, Expected %s",
      func, ltype_name(syms -> cell[i] -> type), ltype_name(LVAL_SYM));
  }

  LASSERT(a, (syms -> count == a -> count -1) , "Funtion %s passed incorrect number of values. Got %i, Expected %i ",
    syms -> count, a -> count -1);
  for (int i = 0; i < syms -> count; ++i)
  {
    // If 'def' define in globally. If 'put' define in locally
    if (strcmp(func, "def") == 0) {
      lenv_def(e, syms -> cell[i], a -> cell[i+1]);
    }

    if (strcmp(func, "=")   == 0) {
      lenv_put(e, syms -> cell[i], a -> cell[i+1]);
    }
  }
  lval_del(a);
  return lval_sexpr();
}

lval* builtin_lambda(lenv* e, lval* a){
  // check 2 args, both q -expr
  LASSERT_NUM("\\", a, 2);
  LASSERT_TYPE("\\", a, 0, LVAL_QEXPR);
  LASSERT_TYPE("\\", a, 1, LVAL_QEXPR);

  // check first q expr only contains symbols
  for (int i = 0; i < a -> cell[0] -> count; i++)
  {
    LASSERT(a, (a -> cell[0] -> cell[i] -> type == LVAL_SYM),
      "Cannot define non symbol. Got %s, Expected %s",
      ltype_name(a -> cell[0] -> cell[i] -> type),
      ltype_name(LVAL_SYM));
  }
  // pop first 2 args and make lval lambda
  lval* formals = lval_pop(a, 0);
  lval* body = lval_pop(a, 0);

  lval_del(a);

  return lval_lambda(formals, body);
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

  //def function to define variables/ functions
  lenv_add_builtin(e, "\\",  builtin_lambda);
  lenv_add_builtin(e, "def", builtin_def);
  lenv_add_builtin(e, "=", builtin_put);

  /* Mathematical Functions */
  lenv_add_builtin(e, "+", builtin_add);
  lenv_add_builtin(e, "-", builtin_sub);
  lenv_add_builtin(e, "*", builtin_mul);
  lenv_add_builtin(e, "/", builtin_div);
}