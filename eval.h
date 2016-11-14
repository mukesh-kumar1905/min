#include "lval.h"

lval* lval_eval_sexpr(lval*);
lval* lval_eval(lval*);
lval* lval_pop(lval*, int);
lval* lval_take(lval*, int);
lval* calculate(lval*, char*);
lval* calculate_head(lval*);
lval* calculate_tail(lval*);
lval* calculate_list(lval*);
lval* calculate_eval(lval*);
lval* lval_join(lval*, lval*);
lval* calculate_join(lval*);
lval* calculate_op(lval*, char*);
