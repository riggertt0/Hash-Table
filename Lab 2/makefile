all: main.cpp HashTable.cpp HashFunctions.cpp
	nasm -f elf64 HashFunctions.asm
	g++ main.cpp HashTable.cpp -Wall -c -mavx2 -msse4.2 -faligned-new -O3 -pg
	g++ main.o HashTable.o HashFunctions.o -no-pie -mavx2 -msse4.2 -faligned-new -O3 -pg
	./a.out > /dev/null
	gprof ./a.out > profile
	gprof2dot ./profile | dot -Tsvg -o output.svg
