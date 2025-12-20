CC=gcc
CFLAGS= -Wall -Wextra -O2
TARGET=eidos

SRC = $(shell find src -name '*.c')
OBJ = $(patsubst src/%.c, builds/%.o, $(SRC))

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

builds/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf builds $(TARGET)