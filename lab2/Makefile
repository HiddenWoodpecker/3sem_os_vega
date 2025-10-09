CC=gcc
CFLAGS=-Wall -Wextra -std=c99
TARGET=LS
SOURCES=LS.c

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)

.PHONY: clean
