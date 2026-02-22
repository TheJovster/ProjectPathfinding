#include "Application.h"

int main() {
    constexpr int gridWidth = 30;
    constexpr int gridHeight = 30;
    constexpr int cellSize = 20;

    Application app(gridWidth, gridHeight, cellSize);
    app.run();

    return 0;
}
