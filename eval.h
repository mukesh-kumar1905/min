#include "lval.h"

lval* lval_eval_sexpr(lenv*, lval*);
lval* lval_eval(lenv*, lval*);

lval* lval_pop(lval*, int);
lval* lval_take(lval*, int);

lval* builtin_add(lenv* e, lval* a);
lval* builtin_sub(lenv* e, lval* a);
lval* builtin_mul(lenv* e, lval* a);
lval* builtin_div(lenv* e, lval* a);
lval* builtin_head(lenv*, lval*);
lval* builtin_tail(lenv*, lval*);
lval* builtin_list(lenv*, lval*);
lval* builtin_eval(lenv*, lval*);
lval* lval_join(lval*, lval*);
lval* builtin_join(lenv*, lval*);
lval* builtin_op(lenv*, lval*, char*);

void lenv_add_builtins(lenv*);