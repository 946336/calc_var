
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -O2 -g

LDFLAGS = -lm

NOLINK = -c

calc: main.c utility.o binding.o value.o env.o ast.o operator.o subexp.o \
		tokenize.o parse.o basis.o relop.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

binding.o: binding.c binding.h utility.h
	$(CC) $(CFLAGS) $(NOLINK) -o $@ $< $(LDFLAGS)

value.o: value.c value.h utility.h
	$(CC) $(CFLAGS) $(NOLINK) -o $@ $< $(LDFLAGS)

ast.o: ast.c ast.h value.h env.h
	$(CC) $(CFLAGS) $(NOLINK) -o $@ $< $(LDFLAGS)

operator.o: operator.c operator.h
	$(CC) $(CFLAGS) $(NOLINK) -o $@ $< $(LDFLAGS)

subexp.o: subexp.c subexp.h ast.h value.h env.h
	$(CC) $(CFLAGS) $(NOLINK) -o $@ $< $(LDFLAGS)

tokenize.o: tokenize.c tokenize.h value.h operator.h
	$(CC) $(CFLAGS) $(NOLINK) -o $@ $< $(LDFLAGS)

parse.o: parse.c parse.h value.h env.h tokenize.h
	$(CC) $(CFLAGS) $(NOLINK) -o $@ $< $(LDFLAGS)

env.o: env.c env.h value.h binding.h
	$(CC) $(CFLAGS) $(NOLINK) -o $@ $< $(LDFLAGS)

basis.o: basis.c basis.h value.h env.h
	$(CC) $(CFLAGS) $(NOLINK) -o $@ $< $(LDFLAGS)

relop.o: relop.c relop.h
	$(CC) $(CFLAGS) $(NOLINK) -o $@ $< $(LDFLAGS)

solution: 
	$(CC) $(CFLAGS) -o $@ solution.c $(LDFLAGS)

clean:
	rm -f *.o test solution
