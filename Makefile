CC     = gcc
CFLAGS = -Wall -Wextra -g -Isrc -MMD -MP -Iinclude
BUILD  = build

# Base objects (always built)
OBJ = $(BUILD)/main.o        \
      $(BUILD)/maze.o        \
      $(BUILD)/stack.o       \
      $(BUILD)/heap.o        \
      $(BUILD)/linked_list.o \
      $(BUILD)/backtrack.o   \
      $(BUILD)/renderer.o    \
      $(BUILD)/pathfind.o

# Optional graphical renderer — build with: make WITH_GFX=1
# Requires raylib: sudo apt install libraylib-dev
ifdef WITH_GFX
  CFLAGS  += -DWITH_GFX
  GFX_OBJ  = $(BUILD)/renderer_gfx.o
  LDFLAGS  = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
else
  GFX_OBJ  =
  LDFLAGS  =
endif

OBJ += $(GFX_OBJ)

-include $(OBJ:.o=.d)

# Top-level targets
.PHONY: all test test-visual clean

all: maze

maze: $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Automated tests
test: $(BUILD)/test_stack $(BUILD)/test_linked_list $(BUILD)/test_backtrack
	@echo "=== test_stack ==="
	./$(BUILD)/test_stack
	@echo "=== test_linked_list ==="
	./$(BUILD)/test_linked_list
	@echo "=== test_backtrack ==="
	./$(BUILD)/test_backtrack

$(BUILD)/test_stack: tests/auto/test_stack.c $(BUILD)/stack.o | $(BUILD)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD)/test_linked_list: tests/auto/test_linked_list.c $(BUILD)/linked_list.o | $(BUILD)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD)/test_backtrack: tests/auto/test_backtrack.c \
                         $(BUILD)/stack.o $(BUILD)/linked_list.o \
                         $(BUILD)/maze.o $(BUILD)/backtrack.o \
                         $(BUILD)/renderer.o $(BUILD)/heap.o \
                         $(BUILD)/pathfind.o $(GFX_OBJ) | $(BUILD)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Visual tests
test-visual: $(BUILD)/visual_test_maze $(BUILD)/visual_test_backpack
	./$(BUILD)/visual_test_maze mazes/maze_10x10.txt
	./$(BUILD)/visual_test_backpack

$(BUILD)/visual_test_maze: tests/visual/visual_test_maze.c \
                           $(BUILD)/maze.o $(BUILD)/backtrack.o \
                           $(BUILD)/renderer.o $(BUILD)/stack.o \
                           $(BUILD)/linked_list.o $(BUILD)/heap.o \
                           $(BUILD)/pathfind.o $(GFX_OBJ) | $(BUILD)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILD)/visual_test_backpack: tests/visual/visual_test_backpack.c \
                               $(BUILD)/linked_list.o | $(BUILD)
	$(CC) $(CFLAGS) $^ -o $@

# Compile rules
$(BUILD)/%.o: src/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: src/maze/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: src/engine/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: src/structures/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD):
	mkdir -p $(BUILD)

# Clean
clean:
	rm -rf $(BUILD) maze
