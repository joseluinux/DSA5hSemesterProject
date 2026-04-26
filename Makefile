CC     = gcc
CFLAGS = -Wall -Wextra -g -Isrc -MMD -MP -Iinclude
BUILD  = build

# Main objects
OBJ = $(BUILD)/main.o       \
      $(BUILD)/maze.o       \
      $(BUILD)/stack.o      \
      $(BUILD)/linked_list.o \
      $(BUILD)/backtrack.o  \
      $(BUILD)/renderer.o

-include $(OBJ:.o=.d)

# Top-level targets
.PHONY: all test test-visual clean

all: maze

maze: $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

# Automated tests
test: $(BUILD)/test_stack $(BUILD)/test_linked_list $(BUILD)/test_backtrack
	@echo "=== test_stack ==="
	./$(BUILD)/test_stack
	@echo "=== test_linked_list ==="
	./$(BUILD)/test_linked_list
	@echo "=== test_backtrack ==="
	./$(BUILD)/test_backtrack

$(BUILD)/test_stack: ../DSA5hSemesterProject/tests/auto $(BUILD)/stack.o | $(BUILD)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD)/test_linked_list: ../DSA5hSemesterProject/tests/auto $(BUILD)/linked_list.o | $(BUILD)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD)/test_backtrack: ../DSA5hSemesterProject/tests/auto \
                         $(BUILD)/stack.o $(BUILD)/linked_list.o \
                         $(BUILD)/maze.o $(BUILD)/backtrack.o $(BUILD)/renderer.o | $(BUILD)
	$(CC) $(CFLAGS) $^ -o $@

# Visual tests
test-visual: $(BUILD)/visual_test_maze $(BUILD)/visual_test_backpack
	./$(BUILD)/visual_test_maze mazes/maze_10x10.txt
	./$(BUILD)/visual_test_backpack

$(BUILD)/visual_test_maze: ../DSA5hSemesterProject/tests/visual \
                           $(BUILD)/maze.o $(BUILD)/backtrack.o $(BUILD)/renderer.o \
                           $(BUILD)/stack.o $(BUILD)/linked_list.o | $(BUILD)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD)/visual_test_backpack: ../DSA5hSemesterProject/tests/visual \
                               $(BUILD)/linked_list.o | $(BUILD)
	$(CC) $(CFLAGS) $^ -o $@

# Compile rules
$(BUILD)/%.o: src/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: ../DSA5hSemesterProject/src/maze | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: ../DSA5hSemesterProject/src/engine | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: ../DSA5hSemesterProject/src/structures | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD):
	mkdir -p $(BUILD)

# Clean
clean:
	rm -rf $(BUILD) maze
