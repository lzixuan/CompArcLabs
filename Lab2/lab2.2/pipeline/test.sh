#!/bin/bash
make
riscv64-unknown-elf-g++ -Wa,-march=rv64g 1.cpp -o test1
riscv64-unknown-elf-g++ -Wa,-march=rv64g 2.cpp -o test2
riscv64-unknown-elf-g++ -Wa,-march=rv64g 3.cpp -o test3
riscv64-unknown-elf-g++ -Wa,-march=rv64g 4.cpp -o test4
riscv64-unknown-elf-g++ -Wa,-march=rv64g 5.cpp -o test5
riscv64-unknown-elf-g++ -Wa,-march=rv64g 6.cpp -o test6
riscv64-unknown-elf-g++ -Wa,-march=rv64g 7.cpp -o test7
riscv64-unknown-elf-g++ -Wa,-march=rv64g 8.cpp -o test8
riscv64-unknown-elf-g++ -Wa,-march=rv64g 9.cpp -o test9
riscv64-unknown-elf-g++ -Wa,-march=rv64g 10.cpp -o test10
./Simulation test1
./Simulation test2
./Simulation test3
./Simulation test4
./Simulation test5
./Simulation test6
./Simulation test7
./Simulation test8
./Simulation test9
./Simulation test10
