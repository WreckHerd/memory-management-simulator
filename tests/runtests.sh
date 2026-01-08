#!/bin/bash

echo "Compiling simulator..."
g++ -std=c++17 -I include src/main.cpp src/allocator/allocator.cpp src/cache/cache.cpp src/buddy/buddy.cpp -o memsim

if [ $? -ne 0 ]; then
    echo "Compilation failed!"
    exit 1
fi

echo "Compilation successful. Running tests..."
echo "----------------------------------------"

TESTS=("tests/trace_allocator_bestfit.txt" "tests/trace_allocator_worstfit.txt" "tests/trace_allocator_firstfit.txt" "tests/trace_buddy.txt" "tests/trace_cacheread_lru.txt")

for test_file in "${TESTS[@]}"
do
    if [ -f "$test_file" ]; then
        echo "Running test: $test_file"
        
        # Run simulator and save output to a results file
        outfile="${test_file%.txt}.result"
        ./memsim < "$test_file" > "$outfile"
        
        echo "  -> Finished. Output saved to $outfile"
        
        echo "----------------------------------------"
    else
        echo "Warning: $test_file not found."
    fi
done

echo "All tests completed."