CC := gcc
CFLAGS := -Wall -Wextra -O2

SRC_DIRS := $(filter-out bin,$(wildcard */))
TARGETS := $(SRC_DIRS:%/=%)

REQUEST_PARSER := $(request-parser.h)

BIN_DIR := bin

.PHONY: all clean run list $(TARGETS)

all: $(TARGETS)

$(TARGETS):
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(REQUEST_PARSER) $@/main.c -o $(BIN_DIR)/$@

run:
	./$(BIN_DIR)/$(NAME)


list:
	@echo "challenges:"
	@printf " %s\n" $(TARGETS)

clean:
	rm -f $(BIN_DIR)/*