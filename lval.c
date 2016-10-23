#include <stdio.h>

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

// number type for lval
lval lval_num(long x){
  lval v;
  v.type = LVAL_NUM;
  v.num = x;
  return v;
}

//error type for lval
lval lval_err(int x){
  lval v;
  v.type = LVAL_ERR;
  v.err = x;
  return v;
}

// print lval
void lval_print(lval v){
  switch (v.type){
    // in case its a number print it
    case LVAL_NUM: printf("%li", v.num); break;
    // in case of error 
    case LVAL_ERR:
      switch(v.err){
        case LERR_DIV_ZERO:
          printf("Error: Division By Zero!");
          break;
        case LERR_BAD_OP:
          printf("Error: Invalid Operator!");
          break;
        case LERR_BAD_NUM:
          printf("Error: Invalid Number!");
          break;
        default:
          printf("Unknown error, code:%i",v.err);
      }
      break;
  }
}

// print lval followed by newline
void lval_println(lval v) { lval_print(v); putchar('\n'); }