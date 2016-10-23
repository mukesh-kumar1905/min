// Create Enumeration of Possible val Types
enum { LVAL_NUM, LVAL_ERR };

// Create Enumeration of Possible Error Types
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

// lval struct to denote lisp return value enabling return of error or number
typedef struct {
  int type;
  long num;
  int err;
} lval;

lval lval_num(long x);

lval lval_err(int x);

void lval_print(lval v);

void lval_println(lval v);