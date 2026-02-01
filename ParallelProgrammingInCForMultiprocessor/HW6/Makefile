CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -Wshadow -lm -fopenmp -funroll-loops -march=native -O3

EXEC = hw6
SRC_C = F74124066_hw6.c
SRC_CXX = main.cpp

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

.PHONY: all clean

all: $(EXEC)

$(EXEC): $(SRC)
	$(COMPILER) -o $(EXEC) $(SRC) $(CFLAGS)

clean:
	rm -f $(EXEC)
