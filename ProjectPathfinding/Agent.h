#pragma once
#include "Vec2i.h"
#include <vector>
#include <optional>

class Agent {
public:
    Agent();

    void setPath(const std::vector<Vec2i>& path);

    void update(float deltaTime);

    std::optional<Vec2i> getPosition() const;

    bool hasReachedDestination() const;

    void setMoveInterval(float seconds) { m_moveInterval = seconds; }
    float getMoveInterval() const { return m_moveInterval; }

    bool hasPath() const { return !m_path.empty(); }

    void reset();

private:
    std::vector<Vec2i> m_path;
    size_t m_currentPathIndex;
    float m_moveTimer;
    float m_moveInterval;
};