#pragma once
#include <vector>
#include "colors.h"
#include "board.h"
#include "player.h"
#include "tiles.h"

const int MAX_ROUNDS = 9;

class Game {
private:
    Board board;
    std::vector<Player> players;
    std::vector<GameTile> tileQueue;
    int currentTileIndex;
    int roundNumber;
    int globalTurnCounter;
    
    void generateTiles(int numPlayers);
    void placeBonuses(int numPlayers);
    void initialize();
    void playTurn(int playerIdx);
    Position parseCoordinates(const std::string& input);
    
public:
    Game(int numPlayers);
    
    void addPlayer(const Player& p);
    void play();
};