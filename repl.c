#include <stdio.h>
#include <stdlib.h>

// arrow keys work fine on windows but doesnt work on mac and linux
// solution is to use editline and simply fake them functions for windows

/* for windows compilation */
#ifdef _WIN32
#include <string.h>

static char buffer[2048];

/* fake readline function */
char* readline(char* prompt){
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char* cpy = malloc(strlen(buffer)+1);
  strcpy(cpy, buffer);
  cpy(strlen(cpy) - 1) = '\0';
  return cpy;
}

/* fake add history */
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

int main(int i, char** a){
   /* Version and Exit Information */
  puts("min Version 0.1");
  puts("Press Ctrl+c to Exit\n");

  // repl loop
  while (1) {
    char* input = readline("min> ");

    add_history(input);

    printf("so you typed %s \n", input);

    free(input);
  }
  return 0;
}
