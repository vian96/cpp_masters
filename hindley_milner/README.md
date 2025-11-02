# Hindley-Milner function principal type inference

To build everything and run tests, run `./run_all.sh`. You can also do it manually with cmake. Makefile is redundant and is not used.

You can also run `./build/main` for interactive execution: it reads input lines, parses each line as S-expression and prints inferred type. E.g. the other way of running tests is to run `cat ./tests_txts/in.txt | ./build/main > result.txt` and compare result with `./tests_txts/out.txt`
