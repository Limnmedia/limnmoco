
CC := cc
CFLAGS := -Wall -Wextra -g -O2

conv: main.o conv.o
	$(CC) $(CFLAGS) $^ -o conv

main.o: main.cpp
	$(CC) $(CFLAGS) -c $< -o $@

conv.o: conv.cpp conv.hpp
	$(CC) $(CFLAGS) -c $< -o $@

