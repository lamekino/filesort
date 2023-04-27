PROG_NAME = filesort

CC = gcc
CCFLAGS = \
	-std=c89 -ansi -ggdb -Wall -Wextra -Werror -Wno-unused-variable \
	-Wno-unused-parameter -Wno-unused-but-set-variable

SRC = \
	process_directory.c \
	arguments.c

HEADERS = \
	error_handling.h \
	$(SRC:.c=.h)

OBJ = \
	$(SRC:.c=.o) \
	main.o

all: $(PROG_NAME)

clean:
	rm *.o *$(PROG_NAME)

$(PROG_NAME): $(OBJ)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) $(CCFLAGS) -c -o $@ $<

main.o: main.c $(SRC) $(HEADERS)

.PHONY: all clean
