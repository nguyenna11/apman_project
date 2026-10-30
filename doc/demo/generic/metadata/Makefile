CFLAGS=-Wall -Wextra -ggdb3 -Iinclude

SRC=$(wildcard *.c)

all : demo

demo : $(SRC:.c=.o)
	$(CC) $^ -o $@
	@./$@

clean :
	@$(RM) *.o *~ demo
