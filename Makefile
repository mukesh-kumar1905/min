parser:
	rm repl && cc -std=c99 -Wall *.c -ledit -lm -o repl
repl:
	make parser && ./repl
debug:
	(rm repl && cc -std=c99 -g -Wall *.c -ledit -lm -o repl) && (gdb repl)

