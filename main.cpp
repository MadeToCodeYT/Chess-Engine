#include <raylib.h>
#include <iostream>
using namespace std;

const int windowWidth = 800;
const int windowHeight = 800;

int main() {
    InitWindow(windowWidth, windowHeight, "Chess Engine");

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}