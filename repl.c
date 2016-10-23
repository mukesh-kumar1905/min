#include <stdio.h>
#include <stdlib.h>
#include "mpc.h"

/*
  arrow keys work fine on windows but doesnt work on mac and linux
  solution is to use editline and simply fake them functions for windows
*/

// for windows compilation
#ifdef _WIN32
#include <string.h>

static char buffer[2048];

// simple readline function - no need for editline on windows
char* readline(char* prompt){
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char* cpy = malloc(strlen(buffer)+1);
  strcpy(cpy, buffer);
  cpy(strlen(cpy) - 1) = '\0';
  return cpy;
}

// fake add history
void add_history(char* unused) {}

/* Otherwise include the editline headers */
#else
#ifdef __APPLE__
#else
  // the mac imlementaion for readline contains definitions for history
  #include <editline/history.h>
#endif
#include <editline/readline.h>
#endif

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

lval eval(mpc_ast_t*);
lval eval_op(lval, char*, lval);

int main(int i, char** a){
  // polish notation parsers
  mpc_parser_t* Number = mpc_new("number");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr = mpc_new("expr");
  mpc_parser_t* Min = mpc_new("min");

  mpca_lang(MPCA_LANG_DEFAULT,
  "                                                    \
    number   : /-?[0-9]+/;                             \
    operator : '+' | '-' | '*' | '/';                  \
    expr     : <number> | '(' <operator> <expr>+ ')';  \
    min      : /^/ <operator> <expr>+ /$/;             \
  ",
  Number,Operator,Expr,Min);
  // Version and Exit Information
  puts("min Version 0.1");
  puts("Press Ctrl+c to Exit\n");

  // repl loop
  while (1) {
    char* input = readline("min> ");
    add_history(input);

    // parse user input
    mpc_result_t r;
    if(mpc_parse("<stdin>", input, Min, &r)){

      // can print ast for debugging uncomment \/
      // mpc_ast_print(r.output);

      // calculate result
      lval result = eval(r.output);
      lval_println(result);
      mpc_ast_delete(r.output);
    }
    else{
      // otherwise print error
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }

    free(input);
  }
  // Undefine and Delete our Parsers
  mpc_cleanup(4, Number, Operator, Expr, Min);
  return 0;
}

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