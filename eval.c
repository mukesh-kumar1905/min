#include "eval.h"

lval eval(mpc_ast_t* t){
  // if number return directly
  if (strstr(t->tag,"number")) {
    /* Check if there is some error in conversion */
    errno = 0;
    long x = strtol(t -> contents, NULL, 10);
    // overflow situation
    return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
  }

  // else it is an expression
  // the operator is always second child
  char* op = t -> children[1] -> contents;
  // evaluate third child, base for calculations
  lval x = eval(t -> children[2]);
  // iterate over other children and combine
  int i = 3;
  while (strstr(t -> children[i] -> tag,"expr")) {
    x = eval_op(x, op, eval(t -> children[i]));
    i++;
  }

  return x;
}

lval eval_op(lval x, char* op, lval y){
    /* If either value is an error return it */
  if (x.type == LVAL_ERR) { return x; }
  if (y.type == LVAL_ERR) { return y; }

  // otherwise do math
  if(strcmp(op, "+") == 0){ return lval_num(x.num + y.num); }
  if(strcmp(op, "-") == 0){ return lval_num(x.num - y.num); }
  if(strcmp(op, "*") == 0){ return lval_num(x.num * y.num); }
  if(strcmp(op, "/") == 0){
    return y.num == 0 ? lval_err(LERR_DIV_ZERO) : lval_num(x.num / y.num);
  }
  return lval_err(LERR_BAD_OP);
}