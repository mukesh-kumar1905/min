#include "deps/mpc.h"

struct lval;
struct lenv;
typedef struct lval lval;
typedef struct lenv lenv;

// Create Enumeration of Possible val Types
enum { LVAL_ERR, LVAL_NUM, LVAL_SYM, LVAL_FUN, LVAL_SEXPR, LVAL_QEXPR };

typedef lval*(*lcalculate)(lenv*, lval*);

// lval struct to denote lisp value
struct lval {
  int type;

  // basic
  long num;
  char* err;
  char* sym;

  // function
  lcalculate builtin;
  lenv* env;
  lval* formals;
  lval* body;

  // expression
  int count;
  struct lval** cell;
};

// lenv struct to maintain execution environment
struct lenv {
  lenv* par;
  int count;
  char** syms;
  lval** vals;
};

char* ltype_name(int);

lval* lval_num(long);
lval* lval_err(char*, ...);
lval* lval_sym(char*);
lval* lval_sexpr();
lval* lval_fun(lcalculate);
lval* lval_lambda(lval*, lval*);
lval* lval_add(lval*, lval*);

lval* lval_read_num(mpc_ast_t*);
lval* lval_read(mpc_ast_t*);

lval* lval_copy(lval*);
void lval_del(lval*);

lenv* lenv_new(void);
lenv* lenv_copy(lenv*);
void lenv_del(lenv*);

lval* lenv_get(lenv*, lval*);
void lenv_put(lenv*, lval*, lval*);
void lenv_def(lenv*, lval*, lval*);
void lenv_add_builtin(lenv*, char*, lcalculate);

void lval_print(lval*);
void lval_println(lval*);