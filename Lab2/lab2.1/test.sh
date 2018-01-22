#!/bin/bash
make
riscv64-unknown-elf-g++ -Wa,-march=rv64g quicksort.cpp -o quicksort
riscv64-unknown-elf-g++ -Wa,-march=rv64g ackermann.cpp -o ackermann
riscv64-unknown-elf-g++ -Wa,-march=rv64g matrix_mul.cpp -o matrix_mul
riscv64-unknown-elf-gcc -Wa,-march=rv64g add.c -o add
./Simulation add -s
