# 🚗 Car Game

A classic terminal-based **car dodging game** built with C++ for Windows.  
Dodge enemy cars, rack up points, and see how long you can survive!

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](./LICENSE)
[![Language: C++](https://img.shields.io/badge/Language-C++-blue.svg)](https://isocpp.org/)
[![Platform: Windows](https://img.shields.io/badge/Platform-Windows-lightgrey.svg)]()

---

## 📸 Preview

```
 --------------------        CAR GAME
 |     CAR GAME     |        ----------
 --------------------        Score    : 12
1. Instructions             Best     : 18
2. Start Game               Speed Lv : 3
3. Stats
4. Quit                     Controls
                            ---------
                             A key  - Left
                             D key  - Right
                             ESC    - Quit
```

---

## ✨ Features

- 🏎️ Real-time ASCII car dodging gameplay
- 📈 Live score + session high score tracking
- 👾 Up to 3 enemy cars — more appear as you score higher
- 🔥 Difficulty scaling — speed ramps up every 5 points (5 levels)
- 💾 Stats saved to `data.txt` — viewable from the main menu
- ⌨️ Keyboard-driven controls
- 🧹 Smooth terminal rendering with cursor tricks

---

## 🛠️ Build & Run

> **Platform:** Windows only (uses `windows.h` and `conio.h`)

### ⚡ Option 1 — One-Click (Recommended)

Double-click **`build.bat`** — it compiles and launches the game automatically.

> Requires [MinGW-w64 g++](https://www.mingw-w64.org/downloads/) installed and in your `PATH`.

### 🔨 Option 2 — Manual g++ (MinGW)

```bash
g++ -std=c++17 -O2 -o CarGame.exe CarGame.cpp
CarGame.exe
```

### 📦 Option 3 — CMake

```bash
cmake -B build
cmake --build build
.\build\CarGame.exe
```

> Works with both **MinGW (`mingw32-make`)** and **MSVC (Visual Studio)**.

### 🖥️ Option 4 — Visual Studio

1. Create a new **Empty C++ Project**
2. Add `CarGame.cpp` to the project
3. Press **Ctrl+F5** to build and run

---

## 🎮 Controls

| Key        | Action           |
|------------|------------------|
| `A`        | Move car left    |
| `D`        | Move car right   |
| `ESC`      | Quit to menu     |

**Main Menu**

| Option | Action |
|--------|--------|
| `1`    | Instructions |
| `2`    | Start Game |
| `3`    | Stats (view `data.txt` history) |
| `4`    | Quit |

---

## 🗂️ Project Structure

```
carGame/
├── .github/
│   └── workflows/
│       └── build.yml    # Auto-build & release CI
├── CarGame.cpp          # Main game source (all-in-one)
├── CMakeLists.txt       # CMake build definition
├── build.bat            # One-click build & launch (MinGW)
├── .gitignore           # Git ignore rules
├── LICENSE              # MIT License
└── README.md            # Project documentation
```

---

## 🤖 GitHub Actions – Auto Release

Every time you push a **version tag**, GitHub automatically compiles `CarGame.exe` and attaches it to a release so anyone can download and run it — no compiler needed.

### How to publish a release

```bash
git tag v1.0
git push origin v1.0
```

That's it! GitHub Actions will:
1. Compile `CarGame.exe` using MinGW on Windows
2. Create a **GitHub Release** named `Car Game v1.0`
3. Attach the ready-to-run `CarGame.exe` to the release

> You can also trigger a build manually from the **Actions** tab on GitHub without creating a tag.

---

## 🚀 How It Works

1. **Enemies** spawn at the top of the road and fall downward each frame.
2. **Your car** stays near the bottom — dodge left or right.
3. Every enemy that passes without a collision **adds +1 to your score**.
4. A **second enemy** activates mid-round; a **third enemy** activates at score 5.
5. Every 5 points the game **speeds up** (5 difficulty levels, max at score 20).
6. A collision triggers **Game Over** — your result is saved to `data.txt`.
7. View your full history any time from **Menu → Stats**.

### 📊 Difficulty Curve

| Score | Speed  | Level |
|-------|--------|-------|
| 0–4   | Slow   | 1     |
| 5–9   | Normal | 2     |
| 10–14 | Fast   | 3     |
| 15–19 | Faster | 4     |
| 20+   | Max    | 5     |

---

## 📋 Notes

- Frame speed is **adaptive** — ranges from 70ms (level 1) to 18ms (level 5).
- Enemy spawn uses a **conflict-check** to prevent unfair overlapping positions.
- Stats are **appended** to `data.txt` (never overwritten) — full history is kept.
- The codebase is a single **`CarGame` class** — no external dependencies.

---

## 📜 License

This project is licensed under the **MIT License**.  
Copyright © 2026 [Rajjit Laishram](https://github.com/rajjitlai)

See [`LICENSE`](./LICENSE) for full details.
