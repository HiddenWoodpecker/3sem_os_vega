CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
TARGETS = mycat mygrep
SOURCES = mycat.c mygrep.c

all: $(TARGETS)

mycat: mycat.c
	$(CC) $(CFLAGS) -o mycat mycat.c

mygrep: mygrep.c
	$(CC) $(CFLAGS) -o mygrep mygrep.c

clean:
	rm -f $(TARGETS)

.PHONY: all clean 