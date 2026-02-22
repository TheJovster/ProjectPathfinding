#pragma once
#include "Vec2i.h"
#include <vector>
#include <optional>

enum class CellType {
    Walkable,
    Obstacle,
    Start,
    End
};

class Grid {
public:
    Grid(int width, int height);

    // Cell queries
    CellType getCellType(const Vec2i& pos) const;
    void setCellType(const Vec2i& pos, CellType type);
    bool isWalkable(const Vec2i& pos) const;
    bool isInBounds(const Vec2i& pos) const;

    // Start/End management
    std::optional<Vec2i> getStartPosition() const;
    std::optional<Vec2i> getEndPosition() const;
    void setStart(const Vec2i& pos);
    void setEnd(const Vec2i& pos);

    // Grid properties
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

    // Clear the grid
    void clear();

private:
    int m_width;
    int m_height;
    std::vector<std::vector<CellType>> m_cells;

    // Cache start/end positions to avoid searching
    std::optional<Vec2i> m_startPos;
    std::optional<Vec2i> m_endPos;

    // Helper to update special case grids - Start and End
    void updateSpecialCell(CellType type, std::optional<Vec2i>& cache, const Vec2i& newPos);
};