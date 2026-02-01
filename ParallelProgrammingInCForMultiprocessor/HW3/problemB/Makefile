CC := mpicc
CFLAGS := -Wall -O2
TARGET := hw3b

all: $(TARGET)

$(TARGET): F74124066_hw3B.c
	$(CC) -o $@ $^ $(CFLAGS)

judge:
	chmod +x judge.sh
	bash judge.sh

clean:
	rm -f $(TARGET)
	rm -rf output
