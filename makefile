CC = gcc
BIN_DIR = bin
SRC_DIR = src

SOURCES = $(wildcard $(SRC_DIR)/*.c)

PLI: $(SOURCES)
	$(CC) $(SOURCES) -o $(BIN_DIR)/interpreter -Iinclude

PLI_mem: $(SOURCES)
	$(CC) $(SOURCES) -g -o $(BIN_DIR)/interpreter_mem -Iinclude

PLI_mem_start:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./$(BIN_DIR)/interpreter_mem

clean:
	rm function_names.txt function_names.json
	rm -rf $(BIN_DIR)/*

.PHONY: clean