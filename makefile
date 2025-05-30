build:
	clang++ -Wall ~/Documents/'Summer Research'/Code/main.cpp -o main
builddb:
	clang++ -Wall -g -O0 ~/Documents/'Summer Research'/Code/main.cpp -o main
run:
	./main
