CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -Wshadow -lm -fopenmp -funroll-loops -march=native -std=c++17 -O3 -ffast-math

EXEC = hw7
SRC_C = F74124066_hw7.c
SRC_CXX = F74124066_hw7.cpp

ifneq ($(wildcard $(SRC_CXX)),)
	SRC = $(SRC_CXX)
	COMPILER = $(CXX)
	EXT = cpp
else ifneq ($(wildcard $(SRC_C)),)
	SRC = $(SRC_C)
	COMPILER = $(CC)
	EXT = c
else
	$(error No main.c or main.cpp found)
endif

.PHONY: compile clean run

compile: $(EXEC)

$(EXEC): $(SRC)
	$(COMPILER) -o $(EXEC) $(SRC) $(CFLAGS)

run:
	./$(EXEC)

clean:
	rm -f $(EXEC)
