parser:
	cc -std=c99 -Wall *.c -ledit -lm -o repl
repl:
	make parser && ./repl

