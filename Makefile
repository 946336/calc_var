
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -O0 -g

LDFLAGS = -lm

NOLINK = -c

calc: main.c utility.o binding.o value.o env.o ast.o operator.o subexp.o \
		tokenize.o parse.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

binding.o: binding.c binding.h utility.o
	$(CC) $(CFLAGS) $(NOLINK) -o $@ $< $(LDFLAGS)

value.o: value.c value.h utility.o
	$(CC) $(CFLAGS) $(NOLINK) -o $@ $< $(LDFLAGS)

ast.o: ast.c ast.h value.o env.o
	$(CC) $(CFLAGS) $(NOLINK) -o $@ $< $(LDFLAGS)

operator.o: operator.c operator.h
	$(CC) $(CFLAGS) $(NOLINK) -o $@ $< $(LDFLAGS)

subexp.o: subexp.c subexp.h ast.o value.o env.o
	$(CC) $(CFLAGS) $(NOLINK) -o $@ $< $(LDFLAGS)

tokenize.o: tokenize.c tokenize.h value.o operator.o
	$(CC) $(CFLAGS) $(NOLINK) -o $@ $< $(LDFLAGS)

parse.o: parse.c parse.h value.o env.o tokenize.o
	$(CC) $(CFLAGS) $(NOLINK) -o $@ $< $(LDFLAGS)

env.o: env.c env.h value.o binding.o 
	$(CC) $(CFLAGS) $(NOLINK) -o $@ $< $(LDFLAGS)

solution: 
	$(CC) $(CFLAGS) -o $@ solution.c $(LDFLAGS)

clean:
	rm -f *.o test solution