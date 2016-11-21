repl:
	(rm -rf repl) && (cc -std=c99 -Wall *.c deps/*.c -ledit -lm -o repl) && ./repl
debug:
	(rm -rf repl && cc -std=c99 -g -Wall *.c deps/*.c -ledit -lm -o repl) && (gdb repl)
clear:
	rm -rf repl*
