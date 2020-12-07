all: build
clean: docs_clean

build:
	g++ -g -O2 -o main main.cpp -lsimlib -lm

run: build
	./main

docs_clean:
	rm -f main