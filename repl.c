#include <stdio.h>
#include <stdlib.h>
#include "mpc.h"
#include "lval.h"

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
    char* input = readline("min> ");
    add_history(input);

    // parse user input
    mpc_result_t r;
    if(mpc_parse("<stdin>", input, Min, &r)){

      // can print ast for debugging uncomment \/
      // mpc_ast_print(r.output);

      // calculate result
      // lval result = eval(r.output);
      lval* x = lval_read(r.output);
      lval_println(x);
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