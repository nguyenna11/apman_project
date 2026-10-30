CFLAGS=-Wall -Wextra -ggdb3 -Iinclude

SRC=$(wildcard *.c)

all : demo

demo : list.o main.o
	$(CC) $^ -o $@
	@./$@

two-lists : list.o two-lists.o
	$(CC) $^ -o $@

clean :
	@$(RM) *.o *~ demo two-lists
