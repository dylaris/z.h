BIN_DIR := output
SRC_DIR := examples
CC      := cc
CFLAGS  := -Wall -Wextra -I. -g

TARGETS := $(BIN_DIR)/array $(BIN_DIR)/deque $(BIN_DIR)/log $(BIN_DIR)/lsdot

all: $(TARGETS)

$(BIN_DIR):
	mkdir -p $@

$(BIN_DIR)/%: $(SRC_DIR)/%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -rf $(BIN_DIR)

.PHONY: all clean
