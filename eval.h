#include "lval.h"

lval* lval_eval_sexpr(lenv*, lval*);
lval* lval_eval(lenv*, lval*);

lval* lval_pop(lval*, int);
lval* lval_take(lval*, int);

lval* builtin_add(lenv*, lval*);
lval* builtin_sub(lenv*, lval*);
lval* builtin_mul(lenv*, lval*);
lval* builtin_div(lenv*, lval*);
lval* builtin_head(lenv*, lval*);
lval* builtin_tail(lenv*, lval*);
lval* builtin_list(lenv*, lval*);
lval* builtin_eval(lenv*, lval*);
lval* lval_join(lval*, lval*);
lval* builtin_join(lenv*, lval*);
lval* builtin_op(lenv*, lval*, char*);
lval* builtin_def(lenv*, lval*);

void lenv_add_builtins(lenv*);

// macro to check if condition is met, if not delete lval and return error
#define LASSERT(args, cond, fmt, ...) if(!cond){ lval_del(args); return lval_err(fmt, ##__VA_ARGS__); }