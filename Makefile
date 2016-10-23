parser:
	cc -std=c99 -Wall *.c -ledit -lm -o repl
repl:
	rm -rf repl && make parser && ./repl
debug:
	(rm -rf repl && cc -std=c99 -g -Wall *.c -ledit -lm -o repl) && (gdb repl)

