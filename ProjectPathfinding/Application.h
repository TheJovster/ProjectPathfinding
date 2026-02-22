// Application.h
#pragma once
#include "Grid.h"
#include "Pathfinder.h"
#include "Agent.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <sstream>

enum class PlacementMode {
    Obstacle,
    Start,
    End,
    Navigate
};

class Application {
public:
    Application(int gridWidth, int gridHeight, int cellSize);

    void run();

private:
    Grid m_grid;
    Pathfinder m_pathfinder;
    Agent m_agent;

    sf::RenderWindow m_window;
    float m_cellSize;

    PlacementMode m_currentMode;
    std::vector<Vec2i> m_currentPath;
    sf::Clock m_deltaClock;

    // Pre-built vertex arrays — rebuilt only when the grid or path changes
    sf::VertexArray m_gridVertices;
    sf::VertexArray m_pathVertices;
    bool m_needsRebuild;

    // HUD
    static constexpr int HUD_HEIGHT = 48;
    sf::Font m_font;
    sf::Text m_hudText;
    bool m_fontLoaded;

    void handleEvents();
    void update();
    void render();

    void handleMouseClick(const Vec2i& gridPos, bool isLeftClick);
    void handleKeyPress(sf::Keyboard::Key key);

    // Pathfinds between grid start/end markers
    void recalculatePath();

    // Pathfinds from agent's current position to clicked destination
    void navigateTo(const Vec2i& destination);

    // Returns the agent's current grid position, falling back to the grid start
    Vec2i getAgentOrigin() const;

    void rebuildVertices();
    void appendQuad(sf::VertexArray& vertices, const Vec2i& gridPos, sf::Color color) const;

    void renderAgent();
    void renderHUD();
    std::string getHUDString() const;
    std::string getModeName() const;
    sf::Color getCellColor(CellType type) const;

    bool loadFont();
};