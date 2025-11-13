#include "player.h"
#include <algorithm>

Player::Player(std::string n, PlayerColor c, int i) : name(n), color(c), id(i), swapCoupons(1) {}

std::string Player::getName() const { return name; }
PlayerColor Player::getColor() const { return color; }
std::string Player::getColorName() const { return ::getColorName(color); }
std::string Player::getColoredName() const { 
    return colorText(name + " (" + ::getColorName(color) + ")", color); 
}

void Player::setStartPos(Position pos) { 
    startPos = pos;
    territory.insert(pos);
}

Position Player::getStartPos() const { return startPos; }

void Player::addToTerritory(const std::vector<Position>& positions) {
    for (int i = 0; i < positions.size(); i++) {
        territory.insert(positions[i]);
    }
}

void Player::removeFromTerritory(const Position& pos) {
    territory.erase(pos);
}

bool Player::hasPosition(const Position& pos) const {
    return territory.find(pos) != territory.end();
}

std::set<Position> Player::getTerritory() const { return territory; }

int Player::getSwapCoupons() const { return swapCoupons; }

void Player::addSwapCoupon() { swapCoupons++; }

bool Player::useSwapCoupon() {
    if (swapCoupons > 0) {
        swapCoupons--;
        return true;
    }
    return false;
}

int Player::getLargestSquare() const {
    int maxSize = 0;
    std::set<Position>::iterator it;
    for (it = territory.begin(); it != territory.end(); it++) {
        Position pos = *it;
        for (int size = 1; size <= 20; size++) {
            bool valid = true;
            for (int i = 0; i < size && valid; i++) {
                for (int j = 0; j < size && valid; j++) {
                    Position check(pos.x + i, pos.y + j);
                    if (territory.find(check) == territory.end()) {
                        valid = false;
                    }
                }
            }
            if (valid) {
                if (size > maxSize) maxSize = size;
            } else {
                break;
            }
        }
    }
    return maxSize;
}