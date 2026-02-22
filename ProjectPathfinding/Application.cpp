#include "Application.h"
#include "Config.h"
#include <iostream>

Application::Application(int gridWidth, int gridHeight, int cellSize)
    : m_grid(gridWidth, gridHeight)
    , m_cellSize(static_cast<float>(cellSize))
    , m_currentMode(PlacementMode::Obstacle)
    , m_window(sf::VideoMode(gridWidth* cellSize, gridHeight* cellSize + HUD_HEIGHT),
        "A* Pathfinding Visualizer")
    , m_gridVertices(sf::Quads)
    , m_pathVertices(sf::Quads)
    , m_needsRebuild(true)
    , m_fontLoaded(false)
{
    m_grid.setStart({ 0, 0 });
    m_grid.setEnd({ gridWidth - 1, gridHeight - 1 });

    m_fontLoaded = loadFont();

    if (m_fontLoaded) {
        m_hudText.setFont(m_font);
        m_hudText.setCharacterSize(14);
        m_hudText.setFillColor(Config::HUD_TEXT);
    }

    recalculatePath();
}

bool Application::loadFont() {
    for (const auto& path : Config::FONT_PATHS) {
        if (m_font.loadFromFile(path)) {
            return true;
        }
    }

    std::cerr << "Warning: No system font found. HUD will not be displayed.\n";
    return false;
}

void Application::run() {
    while (m_window.isOpen()) {
        handleEvents();
        update();
        render();
    }
}

void Application::handleEvents() {
    sf::Event event;
    while (m_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            m_window.close();
        }

        if (event.type == sf::Event::MouseButtonPressed) {
            int gridX = static_cast<int>(event.mouseButton.x / m_cellSize);
            int gridY = static_cast<int>(event.mouseButton.y / m_cellSize);
            Vec2i gridPos = { gridX, gridY };

            bool isLeftClick = (event.mouseButton.button == sf::Mouse::Left);
            handleMouseClick(gridPos, isLeftClick);
        }

        if (event.type == sf::Event::KeyPressed) {
            handleKeyPress(event.key.code);
        }
    }
}

void Application::handleMouseClick(const Vec2i& gridPos, bool isLeftClick) {
    // Reject clicks outside the grid (including the HUD area)
    if (!m_grid.isInBounds(gridPos)) {
        return;
    }

    if (isLeftClick) {
        switch (m_currentMode) {
        case PlacementMode::Obstacle:
            if (m_grid.getCellType(gridPos) == CellType::Walkable) {
                m_grid.setCellType(gridPos, CellType::Obstacle);
                recalculatePath();
            }
            break;

        case PlacementMode::Start:
            m_grid.setStart(gridPos);
            recalculatePath();
            break;

        case PlacementMode::End:
            m_grid.setEnd(gridPos);
            recalculatePath();
            break;

        case PlacementMode::Navigate:
            if (m_grid.isWalkable(gridPos)) {
                navigateTo(gridPos);
            }
            break;
        }
    }
    else {
        if (m_grid.getCellType(gridPos) == CellType::Obstacle) {
            m_grid.setCellType(gridPos, CellType::Walkable);
            recalculatePath();
        }
    }
}

void Application::handleKeyPress(sf::Keyboard::Key key) {
    switch (key) {
    case sf::Keyboard::Num1:
        m_currentMode = PlacementMode::Obstacle;
        break;

    case sf::Keyboard::Num2:
        m_currentMode = PlacementMode::Start;
        break;

    case sf::Keyboard::Num3:
        m_currentMode = PlacementMode::End;
        break;

    case sf::Keyboard::Num4:
        m_currentMode = PlacementMode::Navigate;
        break;

    case sf::Keyboard::D:
    {
        bool diagonal = !m_pathfinder.getAllowDiagonal();
        m_pathfinder.setAllowDiagonal(diagonal);
        recalculatePath();
        break;
    }

    case sf::Keyboard::Space:
        recalculatePath();
        break;

    case sf::Keyboard::Escape:
        m_window.close();
        break;

    default:
        break;
    }
}

void Application::recalculatePath() {
    auto startPos = m_grid.getStartPosition();
    auto endPos = m_grid.getEndPosition();

    if (!startPos || !endPos) {
        m_currentPath.clear();
        m_agent.reset();
        m_needsRebuild = true;
        return;
    }

    m_currentPath = m_pathfinder.findPath(m_grid, *startPos, *endPos);

    if (!m_currentPath.empty()) {
        m_agent.setPath(m_currentPath);
    }
    else {
        m_agent.reset();
    }

    m_needsRebuild = true;
}

void Application::navigateTo(const Vec2i& destination) {
    Vec2i origin = getAgentOrigin();

    m_currentPath = m_pathfinder.findPath(m_grid, origin, destination);

    if (!m_currentPath.empty()) {
        m_agent.setPath(m_currentPath);
    }
    else {
        m_agent.reset();
    }

    m_needsRebuild = true;
}

Vec2i Application::getAgentOrigin() const {
    // Use the agent's current position if it's on the grid
    auto agentPos = m_agent.getPosition();
    if (agentPos) {
        return *agentPos;
    }

    // Fall back to grid start marker
    auto startPos = m_grid.getStartPosition();
    if (startPos) {
        return *startPos;
    }

    // Last resort: top-left corner
    return { 0, 0 };
}

void Application::update() {
    float deltaTime = m_deltaClock.restart().asSeconds();
    m_agent.update(deltaTime);
}

void Application::render() {
    if (m_needsRebuild) {
        rebuildVertices();
        m_needsRebuild = false;
    }

    m_window.clear();

    m_window.draw(m_gridVertices);
    m_window.draw(m_pathVertices);

    renderAgent();
    renderHUD();

    m_window.display();
}

void Application::rebuildVertices() {
    const int width = m_grid.getWidth();
    const int height = m_grid.getHeight();

    m_gridVertices.clear();
    m_gridVertices.resize(static_cast<size_t>(width * height * 4));

    size_t vertexIndex = 0;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Vec2i pos = { x, y };
            sf::Color color = getCellColor(m_grid.getCellType(pos));

            float left = x * m_cellSize;
            float top = y * m_cellSize;
            float right = left + m_cellSize - 1.0f;
            float bottom = top + m_cellSize - 1.0f;

            m_gridVertices[vertexIndex + 0] = sf::Vertex({ left,  top }, color);
            m_gridVertices[vertexIndex + 1] = sf::Vertex({ right, top }, color);
            m_gridVertices[vertexIndex + 2] = sf::Vertex({ right, bottom }, color);
            m_gridVertices[vertexIndex + 3] = sf::Vertex({ left,  bottom }, color);

            vertexIndex += 4;
        }
    }

    m_pathVertices.clear();

    for (const Vec2i& pos : m_currentPath) {
        CellType type = m_grid.getCellType(pos);
        if (type == CellType::Start || type == CellType::End) {
            continue;
        }

        appendQuad(m_pathVertices, pos, Config::PATH_COLOR);
    }
}

void Application::appendQuad(sf::VertexArray& vertices, const Vec2i& gridPos, sf::Color color) const {
    float left = gridPos.x * m_cellSize;
    float top = gridPos.y * m_cellSize;
    float right = left + m_cellSize - 1.0f;
    float bottom = top + m_cellSize - 1.0f;

    vertices.append(sf::Vertex({ left,  top }, color));
    vertices.append(sf::Vertex({ right, top }, color));
    vertices.append(sf::Vertex({ right, bottom }, color));
    vertices.append(sf::Vertex({ left,  bottom }, color));
}

void Application::renderAgent() {
    auto pos = m_agent.getPosition();

    if (!pos) {
        return;
    }

    float radius = m_cellSize / 3.0f;

    sf::CircleShape agentCircle(radius);
    agentCircle.setPosition(
        pos->x * m_cellSize + radius,
        pos->y * m_cellSize + radius
    );
    agentCircle.setFillColor(Config::AGENT_COLOR);
    m_window.draw(agentCircle);
}

void Application::renderHUD() {
    if (!m_fontLoaded) {
        return;
    }

    float gridBottom = m_grid.getHeight() * m_cellSize;
    float windowWidth = static_cast<float>(m_window.getSize().x);

    sf::RectangleShape hudBackground(sf::Vector2f(windowWidth, static_cast<float>(HUD_HEIGHT)));
    hudBackground.setPosition(0.0f, gridBottom);
    hudBackground.setFillColor(Config::HUD_BACKGROUND);
    m_window.draw(hudBackground);

    m_hudText.setString(getHUDString());
    m_hudText.setPosition(8.0f, gridBottom + 6.0f);
    m_window.draw(m_hudText);
}

std::string Application::getHUDString() const {
    std::ostringstream hud;

    hud << "[1] Obstacle  [2] Start  [3] End  [4] Navigate  |  Mode: " << getModeName()
        << "  |  [D] Diagonal: " << (m_pathfinder.getAllowDiagonal() ? "ON" : "OFF")
        << "  |  ";

    if (m_currentPath.empty()) {
        hud << "No path";
    }
    else {
        hud << "Path: " << m_currentPath.size() << " nodes";
    }

    hud << "\n[Space] Recalculate  [RMB] Remove  [Esc] Quit";

    return hud.str();
}

std::string Application::getModeName() const {
    switch (m_currentMode) {
    case PlacementMode::Obstacle: return "Obstacle";
    case PlacementMode::Start:    return "Start";
    case PlacementMode::End:      return "End";
    case PlacementMode::Navigate: return "Navigate";
    default:                      return "Unknown";
    }
}

sf::Color Application::getCellColor(CellType type) const {
    switch (type) {
    case CellType::Walkable:  return Config::CELL_WALKABLE;
    case CellType::Obstacle:  return Config::CELL_OBSTACLE;
    case CellType::Start:     return Config::CELL_START;
    case CellType::End:       return Config::CELL_END;
    default:                  return Config::CELL_DEFAULT;
    }
}