CC=gcc
CFLAGS=-Wall -Wextra -std=c99
TARGET=myfork
SOURCES=myfork.c

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)

.PHONY: clean
