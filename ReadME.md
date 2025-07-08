# 🐍 Snake Game in C (Console Version - Windows)

This is a classic Snake Game written in C using console graphics (Windows-specific). The game includes:

- 🐍 Snake movement with linked list
- 🍎 Regular food and 💰 Special food
- 🧱 Walls and optional obstacles
- 🎮 Easy, Medium, and Hard difficulty modes
- ⏸️ Pause, 🏁 Score display, and ⌛ Dynamic speed change

## 💻 Requirements
- Windows OS (because it uses `<conio.h>` and `windows.h`)
- A C compiler like GCC (MinGW on Windows)
- Command-line terminal

## 🕹️ Controls
- **W** - Move Up
- **A** - Move Left
- **S** - Move Down
- **D** - Move Right
- **P** - Pause/Resume
- **Q** - Quit

## 🔧 How to Run
Compile with GCC:
```bash
gcc snake_game.c -o snake_game
./snake_game
