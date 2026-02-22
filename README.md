# A* Pathfinding Visualizer

An interactive A* pathfinding visualizer built with C++17 and SFML. Place obstacles, set start/end points, toggle diagonal movement, and navigate an agent through the grid in real time.

## Features

- **A\* pathfinding** with configurable cardinal (4-direction) and diagonal (8-direction) movement
- **Octile heuristic** for diagonal mode, Manhattan for cardinal — both admissible and consistent
- **Corner-cutting prevention** ensures the agent can't squeeze diagonally between obstacles
- **Player navigation mode** — click to pathfind and walk the agent to any reachable cell
- **Real-time visualization** with efficient `sf::VertexArray` rendering (2 draw calls for the entire grid)
- **On-screen HUD** showing current mode, path length, diagonal status, and all controls

## Controls

| Key | Action |
|-----|--------|
| `1` | Obstacle placement mode |
| `2` | Start point placement mode |
| `3` | End point placement mode |
| `4` | Navigate mode (click to move agent) |
| `D` | Toggle diagonal movement |
| `Space` | Recalculate path |
| `RMB` | Remove obstacle |
| `Esc` | Quit |

## Building

### Requirements

- C++17 compatible compiler
- [SFML 2.x](https://www.sfml-dev.org/download.php)
- Visual Studio 2022+

### Visual Studio

1. Clone the repository
2. Open `ProjectPathfinding.sln`
3. Ensure SFML include/lib paths are configured in project properties
4. Build and run (F5)

> **Note:** SFML DLLs are not included in the repository. Download them from the [SFML website](https://www.sfml-dev.org/download.php) and place them alongside the executable, or install via your system package manager.

## Project Structure

```
ProjectPathfinding/
├── Vec2i.h            — 2D integer vector used throughout the project
├── Config.h           — Centralized visual and application constants
├── Grid.h / .cpp      — Grid data structure with cell types and start/end management
├── Pathfinder.h / .cpp — A* algorithm with diagonal support and octile heuristic
├── Agent.h / .cpp      — Agent that walks a given path over time
├── Application.h / .cpp — SFML application loop, input handling, rendering, and HUD
└── main.cpp            — Entry point
```

## How It Works

The pathfinder uses the A\* search algorithm with the following design decisions:

- **Integer-scaled costs** (10 for cardinal, 14 for diagonal) avoid floating-point comparison issues while preserving the correct √2 ratio
- **Lazy deletion** in the priority queue — stale entries are skipped via a closed set rather than using a decrease-key operation
- **Memory management** uses `std::unique_ptr` with a node pool for automatic cleanup when `findPath` returns
