#!/bin/bash

# Test all input files in test_lexer_main directory
cd "$(dirname "$0")/.."
TEST_DIR="test_lexer_main"
BIN="./bin/pli"
PASSED=0
FAILED=0
TOTAL=0

echo "=========================================="
echo "Testing lexer with all input files"
echo "=========================================="
echo ""

# Compile if needed
if [ ! -f "$BIN" ]; then
    echo "Compiling..."
    make PLI > /dev/null 2>&1
    if [ $? -ne 0 ]; then
        echo "ERROR: Compilation failed!"
        exit 1
    fi
fi

# Test each input file
for file in "$TEST_DIR"/input*.pli; do
    if [ ! -f "$file" ]; then
        continue
    fi
    
    TOTAL=$((TOTAL + 1))
    filename=$(basename "$file")
    
    echo -n "Testing $filename... "
    
    # Modify main.c temporarily to use this file
    sed -i.bak "s|#define input_file \".*\"|#define input_file \"$file\"|" src/main.c
    
    # Recompile with FILE_INPUT
    gcc src/*.c -o "$BIN" -Iinclude -DFILE_INPUT > /dev/null 2>&1
    
    # Run test
    output=$("$BIN" 2>&1)
    exit_code=$?
    
    # Restore main.c
    mv src/main.c.bak src/main.c
    
    if [ $exit_code -eq 0 ]; then
        echo "PASS"
        PASSED=$((PASSED + 1))
    else
        echo "FAIL (exit code: $exit_code)"
        echo "  Output:"
        echo $output
        FAILED=$((FAILED + 1))
    fi
done

echo ""
echo "=========================================="
echo "Results: $PASSED passed, $FAILED failed out of $TOTAL total"
echo "=========================================="

if [ $FAILED -eq 0 ]; then
    exit 0
else
    exit 1
fi

