#!/bin/bash
make
riscv64-unknown-elf-g++ -Wa,-march=rv64g 1.cpp -o test1
riscv64-unknown-elf-g++ -Wa,-march=rv64g 3.cpp -o test3
riscv64-unknown-elf-g++ -Wa,-march=rv64g 5.cpp -o test5
riscv64-unknown-elf-g++ -Wa,-march=rv64g 7.cpp -o test7
riscv64-unknown-elf-g++ -Wa,-march=rv64g 9.cpp -o test9
./sim test1
./sim test3
./sim test5
./sim test7
./sim test9
