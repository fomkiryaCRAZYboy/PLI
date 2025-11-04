CC = gcc
DB = gdb
BIN_DIR = bin
SRC_DIR = src

DEBUG_FLAGS = -g -O0 -Wall -Wextra

SOURCES = $(wildcard $(SRC_DIR)/*.c)

PLI: $(SOURCES)
	$(CC) $(SOURCES) -o $(BIN_DIR)/pli -Iinclude

PLI_debug: $(SOURCES)
	$(CC) $(SOURCES) -g -O0 -Wall -Wextra -o $(BIN_DIR)/pli_debug -Iinclude

PLI_debug_start:
	$(DB) $(BIN_DIR)/pli_debug --tui

PLI_mem_test: $(SOURCES)
	$(CC) $(SOURCES) -g -o $(BIN_DIR)/pli_mem_test -Iinclude

PLI_mem_test_start:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./$(BIN_DIR)/pli_mem_test

clean:
	rm -rf $(BIN_DIR)/*

.PHONY: clean