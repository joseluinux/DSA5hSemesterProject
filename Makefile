CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude

SRCS = src/main.c src/lab.c src/stack.c src/linked_list.c src/dlinked_list.c
OBJS = $(SRCS:.c=.o)
TARGET = lab

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

tests/auto/%.o: tests/auto/%.c
	$(CC) $(CFLAGS) -c $< -o $@

tests/visual/%.o: tests/visual/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# --- Auto tests ---

linked_list_test: tests/auto/linked_list.o src/linked_list.o
	$(CC) $(CFLAGS) -o $@ $^

dlinked_list_test: tests/auto/dlinked_list.o src/dlinked_list.o
	$(CC) $(CFLAGS) -o $@ $^

stack_test: tests/auto/stack.o src/stack.o
	$(CC) $(CFLAGS) -o $@ $^

test: linked_list_test dlinked_list_test stack_test
	./linked_list_test; ./dlinked_list_test; ./stack_test
	rm -f tests/auto/*.o linked_list_test dlinked_list_test stack_test

# --- Visual / interactive testers ---

linked_list_visual: tests/visual/linked_list.o src/linked_list.o
	$(CC) $(CFLAGS) -o $@ $^

dlinked_list_visual: tests/visual/dlinked_list.o src/dlinked_list.o
	$(CC) $(CFLAGS) -o $@ $^

stack_visual: tests/visual/stack.o src/stack.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f src/*.o tests/auto/*.o tests/visual/*.o $(TARGET) \
	      linked_list_test dlinked_list_test stack_test \
	      linked_list_visual dlinked_list_visual stack_visual

.PHONY: clean test
