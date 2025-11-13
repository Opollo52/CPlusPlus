#pragma once
#include "colors.h"
#include <vector>
#include <map>

class Player;

class Board {
private:
    int size;
    std::map<Position, int> grid;
    std::map<Position, char> tileChars;
    std::vector<Bonus> bonuses;
    
public:
    Board(int s);
    
    int getSize() const;
    bool isValidPosition(const Position& pos) const;
    bool isEmpty(const Position& pos) const;
    int getOwner(const Position& pos) const;
    
    void placeTile(const std::vector<Position>& positions, int playerId);
    void placeTile(const std::vector<Position>& positions, int playerId, char tileChar);
    void placeStone(const Position& pos);
    void removeTile(const Position& pos);
    char getTileChar(const Position& pos) const;
    
    void addBonus(const Bonus& bonus);
    std::vector<Bonus*> checkBonusCapture(const std::vector<Position>& newPositions, int playerId);
    const std::vector<Bonus>& getBonuses() const;
    
    void display(const std::vector<Player>& players) const;
};