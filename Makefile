PROG_NAME = filesort

ifeq ($(PREFIX),)
	PREFIX := /usr/local
endif

CC = gcc
ifeq ($(DEBUG),1)
	CCFLAGS = \
		-std=c89 -ansi -ggdb -Wall -Wextra -Werror -Wno-unused-variable \
		-Wno-unused-parameter -Wno-unused-but-set-variable
else
	CCFLAGS = \
		-std=c89 -ansi -Wall -Wextra -Werror -O3
endif


SRC = \
	transform_file.c \
	usage.c \
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

install: all
	install -m 0755 $(PROG_NAME) $(PREFIX)/bin

$(PROG_NAME): $(OBJ)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) $(CCFLAGS) -c -o $@ $<

main.o: main.c $(SRC) $(HEADERS)

.PHONY: all clean install
