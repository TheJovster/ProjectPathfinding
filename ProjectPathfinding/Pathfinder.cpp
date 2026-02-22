#include "Pathfinder.h"
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <memory>

namespace {

    struct PathNode {
        Vec2i pos;
        int gCost;
        int hCost;
        PathNode* parent;

        PathNode(const Vec2i& pos)
            : pos(pos), gCost(0), hCost(0), parent(nullptr)
        {
        }

        int getFCost() const { return gCost + hCost; }
    };

    struct ComparePathNode {
        bool operator()(const PathNode* a, const PathNode* b) const {
            int fA = a->getFCost();
            int fB = b->getFCost();

            if (fA == fB) {
                return a->hCost > b->hCost;
            }
            return fA > fB;
        }
    };

    constexpr Vec2i CARDINAL_DIRS[] = {
        { 0, -1}, { 0, 1}, {-1, 0}, { 1, 0}
    };

    constexpr Vec2i DIAGONAL_DIRS[] = {
        {-1, -1}, { 1, -1}, {-1, 1}, { 1, 1}
    };

    // Attempts to expand a neighbor node. If the neighbor is new, it's created
    // and added to the open set. If it already exists with a higher g-cost,
    // its cost is relaxed and it's re-added to the open set.
    void expandNeighbor(
        const Vec2i& neighborPos,
        int newGCost,
        int hCost,
        PathNode* parent,
        std::priority_queue<PathNode*, std::vector<PathNode*>, ComparePathNode>& openSet,
        std::vector<std::unique_ptr<PathNode>>& allNodes,
        std::unordered_map<Vec2i, PathNode*>& nodeMap)
    {
        auto it = nodeMap.find(neighborPos);

        if (it != nodeMap.end()) {
            // Node already discovered — relax if we found a cheaper path
            PathNode* existing = it->second;
            if (newGCost < existing->gCost) {
                existing->gCost = newGCost;
                existing->parent = parent;
                openSet.push(existing);
            }
        }
        else {
            // New node — create and add to open set
            auto node = std::make_unique<PathNode>(neighborPos);
            node->gCost = newGCost;
            node->hCost = hCost;
            node->parent = parent;

            PathNode* nodePtr = node.get();
            nodeMap[neighborPos] = nodePtr;
            allNodes.push_back(std::move(node));
            openSet.push(nodePtr);
        }
    }

    std::vector<Vec2i> reconstructPath(const PathNode* goalNode) {
        std::vector<Vec2i> path;
        const PathNode* current = goalNode;

        while (current != nullptr) {
            path.push_back(current->pos);
            current = current->parent;
        }

        std::reverse(path.begin(), path.end());
        return path;
    }

} // anonymous namespace

Pathfinder::Pathfinder()
    : m_allowDiagonal(false)
{
}

std::vector<Vec2i> Pathfinder::findPath(const Grid& grid,
    const Vec2i& start, const Vec2i& end)
{
    if (!grid.isInBounds(start) || !grid.isInBounds(end)) {
        return {};
    }

    const int width = grid.getWidth();
    const int height = grid.getHeight();

    // Open set: nodes to evaluate, sorted by lowest f-cost
    std::priority_queue<PathNode*, std::vector<PathNode*>, ComparePathNode> openSet;

    // Owns all allocated nodes — ensures cleanup when the function returns
    std::vector<std::unique_ptr<PathNode>> allNodes;

    // Maps grid positions to their nodes for O(1) lookup
    std::unordered_map<Vec2i, PathNode*> nodeMap;

    // Closed set: nodes already fully evaluated
    std::vector<std::vector<bool>> closedSet(height, std::vector<bool>(width, false));

    // Seed the open set with the start node
    auto startNode = std::make_unique<PathNode>(start);
    startNode->gCost = 0;
    startNode->hCost = heuristic(start, end);

    PathNode* startPtr = startNode.get();
    nodeMap[start] = startPtr;
    allNodes.push_back(std::move(startNode));
    openSet.push(startPtr);

    while (!openSet.empty()) {
        // Pick the node with the lowest f-cost
        PathNode* current = openSet.top();
        openSet.pop();

        // Skip stale entries (node was already evaluated via a cheaper path)
        if (closedSet[current->pos.y][current->pos.x]) {
            continue;
        }

        // Goal reached — reconstruct and return the path
        if (current->pos == end) {
            return reconstructPath(current);
        }

        closedSet[current->pos.y][current->pos.x] = true;

        // Expand cardinal neighbors (up, down, left, right)
        for (const Vec2i& dir : CARDINAL_DIRS) {
            Vec2i neighborPos = current->pos + dir;

            if (!grid.isWalkable(neighborPos) || closedSet[neighborPos.y][neighborPos.x]) {
                continue;
            }

            expandNeighbor(neighborPos, current->gCost + CARDINAL_COST,
                heuristic(neighborPos, end), current,
                openSet, allNodes, nodeMap);
        }

        // Expand diagonal neighbors if enabled
        if (m_allowDiagonal) {
            for (const Vec2i& dir : DIAGONAL_DIRS) {
                Vec2i neighborPos = current->pos + dir;

                if (!grid.isWalkable(neighborPos) || closedSet[neighborPos.y][neighborPos.x]) {
                    continue;
                }

                // Corner-cutting check: both adjacent cardinal cells must be walkable
                // e.g. to move top-left, both "top" and "left" must be open
                Vec2i adjacentA = { current->pos.x + dir.x, current->pos.y };
                Vec2i adjacentB = { current->pos.x, current->pos.y + dir.y };

                if (!grid.isWalkable(adjacentA) || !grid.isWalkable(adjacentB)) {
                    continue;
                }

                expandNeighbor(neighborPos, current->gCost + DIAGONAL_COST,
                    heuristic(neighborPos, end), current,
                    openSet, allNodes, nodeMap);
            }
        }
    }

    // Open set exhausted with no path to goal
    return {};
}

int Pathfinder::heuristic(const Vec2i& a, const Vec2i& b) const {
    return m_allowDiagonal ? octileDistance(a, b) : manhattanDistance(a, b);
}

int Pathfinder::manhattanDistance(const Vec2i& a, const Vec2i& b) {
    return (std::abs(a.x - b.x) + std::abs(a.y - b.y)) * CARDINAL_COST;
}

int Pathfinder::octileDistance(const Vec2i& a, const Vec2i& b) {
    int dx = std::abs(a.x - b.x);
    int dy = std::abs(a.y - b.y);
    return CARDINAL_COST * (dx + dy) + (DIAGONAL_COST - 2 * CARDINAL_COST) * std::min(dx, dy);
}