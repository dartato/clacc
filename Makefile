CC=gcc
CFLAGS=-Wall -Wextra -Werror -Wshadow -std=c99 -pedantic -g -fwrapv

.PHONY: clacc clean
default: clacc
clacc: clacc.c
	$(CC) $(CFLAGS) -o clacc clacc.c parse.c lib/hdict.c lib/xalloc.c

clean:
	rm -Rf clacc
