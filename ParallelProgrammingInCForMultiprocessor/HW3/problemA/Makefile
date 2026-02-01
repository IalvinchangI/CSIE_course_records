CC := mpicc
CFLAGS := -Wall -O2 -lm
TARGET := hw3a

all: $(TARGET)

$(TARGET): F74124066_hw3A.c
	$(CC) -o $@ $^ $(CFLAGS)

judge:
	chmod +x judge.sh
	bash judge.sh

clean:
	rm -f $(TARGET)
	rm -rf output
