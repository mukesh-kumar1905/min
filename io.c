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

char* readinput(){
  char* input = readline("min> ");
  add_history(input);
  return input;
}