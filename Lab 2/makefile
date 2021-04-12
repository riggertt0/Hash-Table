all: main.cpp HashTable.cpp HashFunctions.cpp
	g++ main.cpp HashTable.cpp HashFunctions.cpp -pg -Wall
	./a.out > /dev/null
	gprof ./a.out > profile
	gprof2dot ./profile | dot -Tsvg -o output.svg
