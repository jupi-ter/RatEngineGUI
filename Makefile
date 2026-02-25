CC = gcc
CFLAGS = -std=c99 -Wall -D_FORTIFY_SOURCE=0 -g
LDFLAGS = -lraylib -lm -lpthread -ldl
TARGET = gui
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

rebuild: clean all

.PHONY: all clean rebuild run
