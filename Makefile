.PHONY: all

all: test

CC=g++-6.1.0 -O2 -Wall -Wextra -fconcepts

test.o: test.c++ ipaddress.h
ipaddress.o: ipaddress.c++ ipaddress.h

test.o ipaddress.o: %.o:
	$(CC) -c -o "$@" -x c++ -std=c++17 "$<"

test: test.o ipaddress.o
	g++-6.1.0 -O2 -Wl,-O1 -o "$@" $^

