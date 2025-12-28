#!/bin/bash
# test_all.sh

for file in good.cl stack.cl complex.cl bad.cl; do
    echo "Testing $file..."
    ./lexer $file | ./parser $file 2>&1 | $HOME/cs143/bin/semant $file 2>&1 > official_${file%.cl}.txt
    ./lexer $file | ./parser $file 2>&1 | ./semant $file 2>&1 > my_${file%.cl}.txt
    if diff official_${file%.cl}.txt my_${file%.cl}.txt > /dev/null; then
        echo "$file: PASS"
    else
        echo "$file: FAIL"
        diff official_${file%.cl}.txt my_${file%.cl}.txt | head -20
    fi
done