#include "Grid.h"
#include <cassert>

Grid::Grid(int width, int height)
    : m_width(width)
    , m_height(height)
    , m_cells(height, std::vector<CellType>(width, CellType::Walkable))
    , m_startPos(std::nullopt)
    , m_endPos(std::nullopt)
{
}

void Grid::setCellType(const Vec2i& pos, CellType type) {
    assert(isInBounds(pos) && "Grid coordinates out of bounds");

    if (type == CellType::Start) {
        updateSpecialCell(type, m_startPos, pos);
    }
    else if (type == CellType::End) {
        updateSpecialCell(type, m_endPos, pos);
    }

    m_cells[pos.y][pos.x] = type;
}

void Grid::updateSpecialCell(CellType type, std::optional<Vec2i>& cache, const Vec2i& newPos) {
    if (cache && *cache == newPos) {
        return;
    }

    if (cache) {
        Vec2i old = *cache;
        if (isInBounds(old) && m_cells[old.y][old.x] == type) {
            m_cells[old.y][old.x] = CellType::Walkable;
        }
    }
    cache = newPos;
}

bool Grid::isWalkable(const Vec2i& pos) const {
    if (!isInBounds(pos)) {
        return false;
    }
    CellType type = m_cells[pos.y][pos.x];
    return type == CellType::Walkable || type == CellType::Start || type == CellType::End;
}

bool Grid::isInBounds(const Vec2i& pos) const {
    return pos.x >= 0 && pos.x < m_width && pos.y >= 0 && pos.y < m_height;
}

std::optional<Vec2i> Grid::getStartPosition() const {
    return m_startPos;
}

std::optional<Vec2i> Grid::getEndPosition() const {
    return m_endPos;
}

void Grid::setStart(const Vec2i& pos) {
    setCellType(pos, CellType::Start);
}

void Grid::setEnd(const Vec2i& pos) {
    setCellType(pos, CellType::End);
}

void Grid::clear() {
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            m_cells[y][x] = CellType::Walkable;
        }
    }
    m_startPos = std::nullopt;
    m_endPos = std::nullopt;
}

CellType Grid::getCellType(const Vec2i& pos) const {
    assert(isInBounds(pos) && "Grid coordinates out of bounds");
    return m_cells[pos.y][pos.x];
}