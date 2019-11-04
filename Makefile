SHELL_OBJS = *.c builtin/*.c
CC = gcc
CFLAGS = -D_GNU_SOURCE -g -Wall
SHELL_NAME = rsh
UTILS = cp mv rm ls touch mkdir ln cat xargs more

BUILD = build/
BIN = $(BUILD)bin/

.PHONY = all run clean

all: $(BUILD)$(SHELL_NAME) $(UTILS:%=$(BIN)%)

run: all
	$(BUILD)$(SHELL_NAME)
clean:
	rm $(BUILD) -rf

$(BUILD)$(SHELL_NAME): $(SHELL_OBJS)
	@mkdir -p $(BUILD)
	$(CC) $(SHELL_OBJS) -lreadline $(CFLAGS) -o $(BUILD)$(SHELL_NAME)

$(BIN)%: utils/%.c
	@mkdir -p $(BIN)
	$(CC) $< $(CFLAGS) -o $@