PROG_NAME = filesort
OBJ_DIR = ./obj
INC_DIR = ./include
SRC_DIR = ./src

ifeq ($(PREFIX),)
	PREFIX := /usr/local
endif

CC = gcc
INCLUDE = -I$(INC_DIR)

ifeq ($(DEBUG),1)
	CCFLAGS = \
		-std=c89 -ansi -ggdb -Wall -Wextra -Werror -Wno-unused-variable \
		-Wno-unused-parameter -Wno-unused-but-set-variable -Wno-unused-function\
		-D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=500 -DDEBUG=1
else
	CCFLAGS = \
		-std=c89 -ansi -Wall -Wextra -Werror -O3 \
		-D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=500
endif

SRC = $(shell find $(SRC_DIR) -name "*.c")
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

all: $(PROG_NAME)

clean:
	rm -fr $(OBJ_DIR) *$(PROG_NAME)

install: all
	install -m 0755 $(PROG_NAME) $(PREFIX)/bin

compile_commands.json:
	@bear -- make

$(PROG_NAME): $(OBJ)
	$(CC) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@) || true
	$(CC) $(INCLUDE) $(CCFLAGS) -c -o $@ $<

.PHONY: all clean install compile_commands.json
