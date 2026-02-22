#pragma once
#include <SFML/Graphics/Color.hpp>
#include <string>

// Visual and application configuration — single source of truth
namespace Config {

    // Grid cell colors
    inline const sf::Color CELL_WALKABLE = sf::Color::Green;
    inline const sf::Color CELL_OBSTACLE = sf::Color::Red;
    inline const sf::Color CELL_START = sf::Color::Yellow;
    inline const sf::Color CELL_END = sf::Color::Cyan;
    inline const sf::Color CELL_DEFAULT = sf::Color::White;

    // Path and agent colors
    inline const sf::Color PATH_COLOR = sf::Color::Blue;
    inline const sf::Color AGENT_COLOR = sf::Color::Magenta;

    // HUD colors
    inline const sf::Color HUD_BACKGROUND = sf::Color(30, 30, 30);
    inline const sf::Color HUD_TEXT = sf::Color::White;

    // Font search paths — tried in order, first match wins
    // For distribution, replace with an embedded font via loadFromMemory()
    inline const std::string FONT_PATHS[] = {
        "assets/font.ttf",                          // Bundled with project (preferred)
        "C:/Windows/Fonts/consola.ttf",              // Windows - Consolas
        "C:/Windows/Fonts/arial.ttf",                // Windows - Arial
        "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",  // Debian/Ubuntu
        "/usr/share/fonts/TTF/DejaVuSansMono.ttf",              // Arch Linux
        "/System/Library/Fonts/SFMono.ttf",          // macOS
        "/System/Library/Fonts/Menlo.ttc",           // macOS fallback
    };

}