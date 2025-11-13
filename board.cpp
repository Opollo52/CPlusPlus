#include "board.h"
#include "colors.h"
#include "player.h"
#include <iostream>
#include <string>
#include <map>

using namespace std;

Board::Board(int s) : size(s) {}

int Board::getSize() const { return size; }

bool Board::isValidPosition(const Position& pos) const {
    return pos.x >= 0 && pos.x < size && pos.y >= 0 && pos.y < size;
}

bool Board::isEmpty(const Position& pos) const {
    return grid.find(pos) == grid.end();
}

int Board::getOwner(const Position& pos) const {
    auto it = grid.find(pos);
    return (it != grid.end()) ? it->second : -2;
}

void Board::placeTile(const std::vector<Position>& positions, int playerId) {
    placeTile(positions, playerId, '1' + playerId);
}

void Board::placeTile(const std::vector<Position>& positions, int playerId, char tileChar) {
    for (const auto& pos : positions) {
        grid[pos] = playerId;
        tileChars[pos] = tileChar;
    }
}

void Board::placeStone(const Position& pos) {
    grid[pos] = -1;
}

void Board::removeTile(const Position& pos) {
    grid.erase(pos);
    tileChars.erase(pos);
}

char Board::getTileChar(const Position& pos) const {
    auto it = tileChars.find(pos);
    return (it != tileChars.end()) ? it->second : '?';
}

void Board::addBonus(const Bonus& bonus) {
    bonuses.push_back(bonus);
}

const std::vector<Bonus>& Board::getBonuses() const {
    return bonuses;
}

std::vector<Bonus*> Board::checkBonusCapture(const std::vector<Position>& newPositions, int playerId) {
    std::vector<Bonus*> captured;
    for (int i = 0; i < bonuses.size(); i++) {
        if (!bonuses[i].claimed) {
            Position bp = bonuses[i].pos;
            vector<Position> cardinals;
            cardinals.push_back(Position(bp.x-1, bp.y));
            cardinals.push_back(Position(bp.x+1, bp.y));
            cardinals.push_back(Position(bp.x, bp.y-1));
            cardinals.push_back(Position(bp.x, bp.y+1));
            
            bool allOwned = true;
            for (int j = 0; j < cardinals.size(); j++) {
                if (getOwner(cardinals[j]) != playerId) {
                    allOwned = false;
                    break;
                }
            }
            
            if (allOwned) {
                bonuses[i].claimed = true;
                grid[bp] = playerId;
                captured.push_back(&bonuses[i]);
            }
        }
    }
    return captured;
}

void Board::display(const std::vector<Player>& players) const {
    cout << "\n" << colorText("=== PLATEAU " + to_string(size) + "x" + to_string(size) + " ===", YELLOW) << "\n";
    cout << "    ";
    for (int x = 0; x < size; x++) {
        char col;
        if (x < 26) {
            col = 'A' + x;
        } else {
            col = 'a' + (x - 26);
        }
        cout << col << " ";
    }
    cout << "\n";
    
    for (int y = 1; y <= size; y++) {
        cout << (y < 10 ? " " : "") << y << " ";
        for (int x = 1; x <= size; x++) {
            Position pos(x-1, y-1);
            
            bool isBonus = false;
            for (const auto& bonus : bonuses) {
                if (bonus.pos == pos && !bonus.claimed) {
                    if (bonus.type == TILE_SWAP) cout << "E ";
                    else if (bonus.type == STONE) cout << "P ";
                    else if (bonus.type == STEAL) cout << "V ";
                    isBonus = true;
                    break;
                }
            }
            
            if (!isBonus) {
                int owner = getOwner(pos);
                if (owner == -1) cout << "X ";
                else if (owner >= 0) {
                    char tileChar = getTileChar(pos);
                    cout << colorText(string(1, tileChar), players[owner].getColor()) << " ";
                }
                else cout << ". ";
            }
        }
        cout << "\n";
    }
    
    cout << colorText("Legende:", YELLOW) << " . = vide, " 
         << "X = pierre, "
         << "E = echange, "
         << "P = pierre bonus, "
         << "V = vol, "
         << "* = tuile 1x1\n";
    
    cout << "\n" << colorText("Joueurs:", YELLOW) << " ";
    for (int i = 0; i < players.size(); i++) {
        cout << colorText(to_string(i+1), players[i].getColor()) << "=" << players[i].getColoredName() << " ";
    }
    cout << "\n";
}