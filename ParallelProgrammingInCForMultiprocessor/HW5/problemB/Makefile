CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -Wshadow -lm -pthread -O2

EXEC = hw5B
SRC_C = main.c
SRC_CXX = F74124066_hw5B.cpp

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
