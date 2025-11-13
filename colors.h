#pragma once
#include <iostream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

struct Position {
    int x, y;
    Position(int x = 0, int y = 0) : x(x), y(y) {}
    bool operator<(const Position& other) const {
        return (x < other.x) || (x == other.x && y < other.y);
    }
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
};

enum BonusType { NONE, TILE_SWAP, STONE, STEAL };

struct Bonus {
    Position pos;
    BonusType type;
    bool claimed;
    Bonus(Position p, BonusType t) : pos(p), type(t), claimed(false) {}
};

enum PlayerColor {
    RED, BLUE, GREEN, YELLOW, ORANGE, PURPLE, PINK, BROWN, GRAY, CYAN
};

std::string getColorCode(PlayerColor color);
std::string getColorName(PlayerColor color);
std::string getResetColor();
std::string colorText(const std::string& text, PlayerColor color);
void enableColors();