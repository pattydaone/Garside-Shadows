build:
	clang++ -Wall -I ~/Documents/'Summer Research'/Code ~/Documents/'Summer Research'/Code/main.cpp -o main.o
b:
	clang++ -Wall -I ~/Documents/'Summer Research'/Code ~/Documents/'Summer Research'/Code/main.cpp -o main.o
builddb:
	clang++ -Wall -g -I~/Documents/"Summer Research"/Code -O0 ~/Documents/'Summer Research'/Code/main.cpp -o main.o
bdb:
	clang++ -Wall -g -I~/Documents/"Summer Research"/Code -O0 ~/Documents/'Summer Research'/Code/main.cpp -o main.o
run:
	./main.o
r:
	./main.o
