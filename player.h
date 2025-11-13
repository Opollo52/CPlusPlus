#pragma once
#include "colors.h"
#include <iostream>
#include <vector>
#include <string>
#include <set>

class Player {
private:
    std::string name;
    PlayerColor color;
    int id;
    Position startPos;
    std::set<Position> territory;
    int swapCoupons;
    
public:
    Player(std::string n, PlayerColor c, int i);
    
    std::string getName() const;
    PlayerColor getColor() const;
    std::string getColorName() const;
    std::string getColoredName() const;
    
    void setStartPos(Position pos);
    Position getStartPos() const;
    
    void addToTerritory(const std::vector<Position>& positions);
    void removeFromTerritory(const Position& pos);
    bool hasPosition(const Position& pos) const;
    std::set<Position> getTerritory() const;
    
    int getSwapCoupons() const;
    void addSwapCoupon();
    bool useSwapCoupon();
    
    int getLargestSquare() const;
};