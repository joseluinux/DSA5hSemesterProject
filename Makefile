CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude

SRCS = src/main.c src/lab.c src/stack.c src/linked_list.c src/dlinked_list.c
OBJS = $(SRCS:.c=.o)
TARGET = lab

LIST_OBJS = src/linked_list.o src/dlinked_list.o

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test_visual: tests/test_visual.o $(LIST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

test_auto: tests/test_auto.o $(LIST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

test_stack: tests/test_stack.o src/stack.o
	$(CC) $(CFLAGS) -o $@ $^

tests/%.o: tests/%.c
	$(CC) $(CFLAGS) -c $< -o $@

test: test_auto
	./test_auto; rm -f tests/test_auto.o test_auto

clean:
	rm -f src/*.o tests/*.o $(TARGET) test_visual test_auto test_stack

.PHONY: clean test
