parser:
	cc -std=c99 -Wall parsing.c mpc.c -ledit -lm -o parsing
repl:
	./parsing

