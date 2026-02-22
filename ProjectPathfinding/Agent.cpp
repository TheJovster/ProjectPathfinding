#include "Agent.h"

Agent::Agent()
    : m_currentPathIndex(0)
    , m_moveTimer(0.0f)
    , m_moveInterval(0.1f)
{
}

void Agent::setPath(const std::vector<Vec2i>& path) {
    m_path = path;
    m_currentPathIndex = 0;
    m_moveTimer = 0.0f;
}

void Agent::update(float deltaTime) {
    if (m_path.empty() || hasReachedDestination()) {
        return;
    }

    m_moveTimer += deltaTime;

    while (m_moveTimer >= m_moveInterval && m_currentPathIndex < m_path.size() - 1) {
        m_moveTimer -= m_moveInterval;
        m_currentPathIndex++;
    }
}

std::optional<Vec2i> Agent::getPosition() const {
    if (m_path.empty()) {
        return std::nullopt;
    }
    return m_path[m_currentPathIndex];
}

bool Agent::hasReachedDestination() const {
    if (m_path.empty()) {
        return true;
    }
    return m_currentPathIndex >= m_path.size() - 1;
}

void Agent::reset() {
    m_path.clear();
    m_currentPathIndex = 0;
    m_moveTimer = 0.0f;
}