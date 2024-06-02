#!/bin/bash
CFILES=$(find . -name "*.c" -o -name "*.h" -not -path "./cpputest/*")

for file in $CFILES; do
    clang-format -i "$file"
done