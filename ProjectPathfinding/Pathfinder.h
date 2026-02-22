#pragma once
#include "Grid.h"
#include "Vec2i.h"
#include <vector>

class Pathfinder {
public:
    Pathfinder();

    void setAllowDiagonal(bool allow) { m_allowDiagonal = allow; }
    bool getAllowDiagonal() const { return m_allowDiagonal; }

    std::vector<Vec2i> findPath(const Grid& grid,
        const Vec2i& start, const Vec2i& end);

private:
    bool m_allowDiagonal;

    static constexpr int CARDINAL_COST = 10;
    static constexpr int DIAGONAL_COST = 14;

    int heuristic(const Vec2i& a, const Vec2i& b) const;
    static int manhattanDistance(const Vec2i& a, const Vec2i& b);
    static int octileDistance(const Vec2i& a, const Vec2i& b);
};