#include "lval.h"

lval* lval_eval_sexpr(lenv*, lval*);
lval* lval_eval(lenv*, lval*);
lval* lval_call(lenv*, lval*, lval*);

lval* lval_pop(lval*, int);
lval* lval_take(lval*, int);
lval* lval_join(lval*, lval*);

lval* builtin_add(lenv*, lval*);
lval* builtin_sub(lenv*, lval*);
lval* builtin_mul(lenv*, lval*);
lval* builtin_div(lenv*, lval*);
lval* builtin_head(lenv*, lval*);
lval* builtin_tail(lenv*, lval*);
lval* builtin_list(lenv*, lval*);
lval* builtin_eval(lenv*, lval*);

lval* builtin_join(lenv*, lval*);
lval* builtin_op(lenv*, lval*, char*);
lval* builtin_var(lenv*, lval*, char*);
lval* builtin_def(lenv*, lval*);
lval* builtin_put(lenv*, lval*);
lval* builtin_lambda(lenv*, lval*);

lval* builtin_gt(lenv*, lval*);
lval* builtin_lt(lenv*, lval*);
lval* builtin_ge(lenv*, lval*);
lval* builtin_le(lenv*, lval*);
lval* builtin_ord(lenv*, lval*, char *);
lval* builtin_cmp(lenv*, lval*, char*);
lval* builtin_eq(lenv*, lval*);
lval* builtin_ne(lenv*, lval*);
lval* builtin_if(lenv*, lval*);


void lenv_add_builtins(lenv*);

// asserts
#define LASSERT(args, cond, fmt, ...) if(!cond){ lval_del(args); return lval_err(fmt, ##__VA_ARGS__); }
#define LASSERT_TYPE(func, args, index, expect) \
  LASSERT(args, (args -> cell[index] -> type == expect), \
    "Function '%s' passed incorrect type for argument %i. Got %s, Expected %s.", \
    func, index, ltype_name(args -> cell[index]->type), ltype_name(expect))

#define LASSERT_NUM(func, args, num) \
  LASSERT(args, (args -> count == num), \
    "Function '%s' passed incorrect number of arguments. Got %i, Expected %i.", \
    func, args->count, num)

#define LASSERT_NOT_EMPTY(func, args, index) \
  LASSERT(args, (args -> cell[index] -> count != 0), \
    "Function '%s' passed {} for argument %i.", func, index);