# C++ Chess Engine

A minimal, interactive chess game and engine written in C++ using [raylib](https://www.raylib.com/) for graphics.

## Requirements

- [raylib](https://www.raylib.com/) (headers and libraries available on your system)
- C++17 compiler (tested with g++)
- CMake (recommended, see below)

## Building

### With CMake (recommended)

The included `CMakeLists.txt` configures and builds the project, automatically finding raylib if possible, or letting you point to its location:

> If your setup requires, feel free to modify `CMakeLists.txt`.

```sh
mkdir build
cd build
cmake ..
make
```

**Notes:**

- On **Windows**, set the `RAYLIB_ROOT` environment variable to your raylib folder if it's not in a standard path (should contain `include/` and `lib/`):
  ``` sh
  set RAYLIB_ROOT=C:/path/to/your/raylib
  ```
- On **Linux/macOS**, required libraries (OpenGL, pthread, X11, etc.) are typically found automatically.

### Manual Compilation (g++ directly)

You can also build manually if you prefer, but be sure to include all required raylib/system dependencies:

```
g++ main.cpp -o chess -lraylib -std=c++17
```

**You may need to add extra flags depending on your OS:**

- **Linux example:**
  ```
  g++ main.cpp -o chess -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
  ```
- **Windows example:**
  ```
  g++ main.cpp -o chess -lraylib -lopengl32 -lgdi32 -lwinmm
  ```

## Running the Game

To run (from the build directory if using CMake):

```
./ChessEngine
```

- Use your mouse to select and move pieces.
- On pawn promotion, select your desired piece from the popup window.
- Game displays checkmate and stalemate automatically.

## Project Structure

- `main.cpp` &mdash; Main game loop, rendering, and user input.
- `board.h` &mdash; Board state, move generation, piece rules, move validation (all board logic is header-only).
- `bot.h` &mdash; Contains the chess engine logic (search and evaluation).

*(There are no separate .cpp files for board/bot; all logic is in headers for simplicity.)*

## License

This project is licensed under the MIT License.  
See [LICENSE](LICENSE) for details.

---

Enjoy playing and exploring!