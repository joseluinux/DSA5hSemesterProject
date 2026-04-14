# DSA5hSemesterProject
## 1. Overview

The project consists of developing the logical engine of an archaeological exploration game. The protagonist, a treasure hunter, must traverse an ancient and dangerous labyrinth to find the exit. Along the way, there are valuable treasures and ancient traps that can empty the backpack.

> **Objective:** reach the exit with the **highest accumulated treasure value** possible.

## 2. Game Mechanics

### 🗺️ The Labyrinth
- Represented by a **matrix of up to 40x40** (one-dimensional)
- The layout must be loaded from a **`.txt` file**

| Symbol | Meaning   |
|--------|-----------|
| `#`    | Wall      |
| ` `    | Corridor  |
| `P`    | Character |
| `T`    | Treasure  |
| `A`    | Trap      |
| `S`    | Exit      |

### 🧭 Movement and Pathfinding
- A **Backtracking** algorithm must be implemented to find the exit
- Required because the labyrinth has multiple crossroads where a chosen path may not lead to the exit

### 🎒 The Backpack
- Each found treasure has a **numeric value** (e.g.: 10, 50, 100 coins)
- Treasures are stored in the backpack
- The value is **randomly drawn** in the range of **1 to 100 coins** upon discovery

**Trap Rule:** upon falling into a trap (`A`), the character loses the treasure at the **first position** of the backpack.

**Maximization Strategy:** to minimize losses, treasures of **lower value** must always remain at the first position of the backpack.

### 🖥️ Visual Interface
- At each step (with a small delay), the labyrinth is displayed in **ASCII characters** on the console
- The current backpack contents are printed beside or below the map

## 3. Technical Requirements

- **Language:** C
- **Mandatory Structures:** Stacks, Lists, and Sorting Algorithm (if necessary)
- **Input:** `.txt` file reading for map assembly (dimensions specified within the file itself)
- **Output:**
  - Step-by-step visual display in the terminal
  - Total treasure value upon finding the exit
  - Correct path (entrance → exit) written to an output `.txt` file

## 4. Evaluation Criteria (maximum 10 points)

| Criterion | Description |
|-----------|-------------|
| Pointers and structures | Correct use of pointers and data structures |
| Search algorithm | Backtracking efficiency in the labyrinth |
| Error handling | File not found, empty backpack on trap, etc. |
| Code quality | Indentation, comments, and modularization |
| Visualization | Step-by-step traversal in text or 2D graphic format |
| Oral presentation | Software demonstration and knowledge of the implementation |

### 📦 Deliverables
- C source code of the application
- `.txt` file with a test labyrinth
- `.txt` output file
- `.txt` file with full name and student ID of all members
