parser:
	cc -std=c99 -Wall parsing.c mpc.c -ledit -lm -o repl
repl:
	make parser && ./repl

