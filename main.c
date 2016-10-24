#include <stdio.h>
#include <stdlib.h>
#include "deps/mpc.h"
#include "eval.h"
#include "io.h"

lval eval(mpc_ast_t*);
lval eval_op(lval, char*, lval);

int main(int i, char** a){
  // polish notation parsers
  mpc_parser_t* Number = mpc_new("number");
  mpc_parser_t* Symbol = mpc_new("symbol");
  mpc_parser_t* Sexpr = mpc_new("sexpr");
  mpc_parser_t* Expr = mpc_new("expr");
  mpc_parser_t* Min = mpc_new("min");

  mpca_lang(MPCA_LANG_DEFAULT,
  "                                                    \
    number   : /-?[0-9]+/;                             \
    symbol   : '+' | '-' | '*' | '/';                  \
    sexpr    : '(' <expr>* ')';                        \
    expr     : <number> | <symbol> | <sexpr>;          \
    min      : /^/ <expr>* /$/;                        \
  ",
  Number, Symbol, Sexpr, Expr, Min);
  // Version and Exit Information
  puts("min Version 0.1");
  puts("Press Ctrl+c to Exit\n");

  // repl loop
  while (1) {
    char* input = readinput();

    // parse user input
    mpc_result_t r;
    if(mpc_parse("<stdin>", input, Min, &r)){

      // can print ast for debugging uncomment \/
      // mpc_ast_print(r.output);

      // calculate result
      lval* result = lval_eval(lval_read(r.output));
      lval_println(result);
      lval_del(result);
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
  mpc_cleanup(5, Number, Symbol, Sexpr, Expr, Min);
  return 0;
}