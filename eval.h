#include "mpc.h"
#include "lval.h"

lval eval(mpc_ast_t*);
lval eval_op(lval, char*, lval);