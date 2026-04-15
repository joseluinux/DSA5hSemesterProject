CC = gcc
CFLAGS = -Wall -Wextra -g -IInclude

SRCS = main.c lab.c linked_list.c dlinked_list.c
OBJS = $(SRCS:.c=.o)
TARGET = lab

LIST_OBJS = linked_list.o dlinked_list.o

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

test_visual: Testers/lists/test_visual.o $(LIST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

test_auto: Testers/lists/test_auto.o $(LIST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

Testers/lists/%.o: Testers/lists/%.c
	$(CC) $(CFLAGS) -c $< -o $@

test: test_auto
	./test_auto; rm -f Testers/lists/test_auto.o test_auto

clean:
	rm -f *.o test/*.o $(TARGET) test_visual test_auto

.PHONY: clean test
