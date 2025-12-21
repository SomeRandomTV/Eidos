#!/bin/bash

# Create logs directory if it doesn't exist
mkdir -p logs

# Run make to build the project
echo "Building project..."
make > logs/make.log 2>&1
if [ $? -ne 0 ]; then
    echo "Build failed! Check logs/make.log"
    exit 1
fi

# Find the executable 
EXECUTABLE="./eidos"  

echo "Running tests..."
PASSED=0
FAILED=0

# Loop through all test files
for test_file in test_codes/*; do
    filename=$(basename "$test_file")
    base_name="${filename%.*}"
    expected_file="test_codes_lexemes/${base_name}_output.e"
    
    if [ ! -f "$expected_file" ]; then
        echo "Warning: No expected output for $filename"
        continue
    fi
    
    # Run the executable and capture output and errors
    $EXECUTABLE "$test_file" > "logs/${base_name}.out" 2> "logs/${base_name}.err"
    output=$(cat "logs/${base_name}.out")
    expected=$(cat "$expected_file")
    
    # Compare output
    if [ "$output" = "$expected" ]; then
        echo "✓ $filename passed"
        ((PASSED++))
    else
        echo "✗ $filename failed"
        echo "  Check logs/${base_name}.out and logs/${base_name}.err"
        ((FAILED++))
    fi
done

echo ""
echo "Results: $PASSED passed, $FAILED failed"
echo "Logs saved to logs/"