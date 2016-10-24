#include "deps/mpc.h"

// Create Enumeration of Possible val Types
enum { LVAL_ERR, LVAL_NUM, LVAL_SYM, LVAL_SEXPR };
// Create Enumeration of Possible Error Types
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

// lval struct to denote lisp return value enabling return of error or number
typedef struct lval {
  int type;
  long num;
  char* err;
  char* sym;
  int count;
  struct lval** cell;
} lval;

lval* lval_num(long);
lval* lval_err(char*);
lval* lval_sym(char*);
lval* lval_sexpr();
lval* lval_add(lval*, lval*);

lval* lval_read_num(mpc_ast_t*);
lval* lval_read(mpc_ast_t*);

void lval_del(lval*);


void lval_print(lval*);
void lval_println(lval*);