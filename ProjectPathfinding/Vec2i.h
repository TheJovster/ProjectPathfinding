#pragma once
#include <functional>

struct Vec2i {
    int x;
    int y;

    constexpr Vec2i() : x(0), y(0) {}
    constexpr Vec2i(int x, int y) : x(x), y(y) {}

    constexpr bool operator==(const Vec2i& other) const {
        return x == other.x && y == other.y;
    }

    constexpr bool operator!=(const Vec2i& other) const {
        return !(*this == other);
    }

    constexpr Vec2i operator+(const Vec2i& other) const {
        return { x + other.x, y + other.y };
    }

    constexpr Vec2i operator-(const Vec2i& other) const {
        return { x - other.x, y - other.y };
    }
};

// Hash function so Vec2i can be used as a key in unordered_map/unordered_set
namespace std {
    template <>
    struct hash<Vec2i> {
        size_t operator()(const Vec2i& v) const {
            size_t h1 = std::hash<int>{}(v.x);
            size_t h2 = std::hash<int>{}(v.y);
            return h1 ^ (h2 << 16);
        }
    };
}
