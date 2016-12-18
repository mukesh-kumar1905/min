repl:
	(rm -rf repl) && (cc -std=c99 -Wall *.c deps/*.c -ledit -lm -o repl) && ./repl sugar.min
debug:
	(rm -rf repl && cc -std=c99 -g -Wall *.c deps/*.c -ledit -lm -o repl) && (gdb repl)
clean:
	rm -rf repl*
