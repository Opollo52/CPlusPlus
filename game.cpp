#include "game.h"
#include <iostream>
#include <random>
#include <algorithm>
#include <string>
#include <cctype>
#include <queue>
#include <set>

using namespace std;

Position Game::parseCoordinates(const string& input) {
    if (input.length() < 2) {
        return Position(-1, -1);
    }
    
    char col = input[0];
    int x;
    if (col >= 'A' && col <= 'Z') {
        x = col - 'A';
    } else if (col >= 'a' && col <= 'd') {
        x = 26 + (col - 'a');
    } else {
        return Position(-1, -1);
    }
    
    string rowStr = input.substr(1);
    int y = stoi(rowStr) - 1;
    
    return Position(x, y);
}

Game::Game(int numPlayers) : board(numPlayers <= 4 ? 20 : 30), currentTileIndex(0), roundNumber(1), globalTurnCounter(0) {
    generateTiles(numPlayers);
}

void Game::addPlayer(const Player& p) {
    players.push_back(p);
}

void Game::generateTiles(int numPlayers) {
    vector<GameTile> availableTiles;
    
    for (int i = 0; i < ALL_TILES.size(); i++) {
        availableTiles.push_back(GameTile(ALL_TILES[i]));
    }
    
    int tilesNeeded = (int)((10.67 * numPlayers) + 0.5);
    
    for (int i = 0; i < tilesNeeded; i++) {
        int idx = i % availableTiles.size();
        tileQueue.push_back(availableTiles[idx]);
    }
}

void Game::placeBonuses(int numPlayers) {
    int swapBonuses = (int)(1.5 * numPlayers + 0.99);
    int stoneBonuses = (int)(0.5 * numPlayers + 0.99);
    int stealBonuses = numPlayers;
    
    for (int i = 0; i < swapBonuses; i++) {
        Position pos(5 + i * 3, 5 + i * 2);
        if (board.isValidPosition(pos)) {
            board.addBonus(Bonus(pos, TILE_SWAP));
        }
    }
    
    for (int i = 0; i < stoneBonuses; i++) {
        Position pos(8 + i * 4, 8 + i * 3);
        if (board.isValidPosition(pos)) {
            board.addBonus(Bonus(pos, STONE));
        }
    }
    
    for (int i = 0; i < stealBonuses; i++) {
        Position pos(12 + i * 2, 12 + i * 1);
        if (board.isValidPosition(pos)) {
            board.addBonus(Bonus(pos, STEAL));
        }
    }
}

void Game::initialize() {
    placeBonuses(players.size());
    
    cout << "\n" << colorText("Ordre de jeu determine:", YELLOW) << "\n";
    for (int i = 0; i < players.size(); i++) {
        cout << (i+1) << ". " << players[i].getColoredName() << "\n";
    }
    
    cout << "\n" << colorText("=== PLACEMENT DES EMPLACEMENTS DE DEPART ===", YELLOW) << "\n";
    for (int i = 0; i < players.size(); i++) {
        cout << "\n" << players[i].getColoredName() << ", choisissez votre emplacement de depart:\n";
        board.display(players);
        
        bool validPlacement = false;
        while (!validPlacement) {
            cout << "Position de depart (ex: J10): ";
            string input;
            cin >> input;
            Position start = parseCoordinates(input);
            
            if (start.x == -1 || start.y == -1) {
                cout << colorText("Format invalide! Utilisez le format 'LettreNumero' (ex: J10)", RED) << "\n";
                continue;
            }
            
            if (!board.isValidPosition(start)) {
                cout << colorText("Position invalide! Choisissez une position dans le plateau.", RED) << "\n";
                continue;
            }
            
            if (!board.isEmpty(start)) {
                cout << colorText("Position occupee! Choisissez une position libre.", RED) << "\n";
                continue;
            }
            
            bool tooClose = false;
            for (int j = 0; j < i; j++) {
                Position otherStart = players[j].getStartPos();
                int distance = abs(start.x - otherStart.x) + abs(start.y - otherStart.y);
                if (distance < 3) {
                    tooClose = true;
                    break;
                }
            }
            
            if (tooClose) {
                cout << colorText("Trop proche d'un autre joueur! Minimum 3 cases de distance.", RED) << "\n";
                continue;
            }
            
            players[i].setStartPos(start);
            board.placeTile({start}, i, '1' + i);
            validPlacement = true;
            cout << colorText("Emplacement confirme!", GREEN) << "\n";
        }
    }
    
    cout << "\n" << colorText("Tous les emplacements de depart sont places!", GREEN) << "\n";
    board.display(players);
}

void Game::playTurn(int playerIdx) {
    Player& player = players[playerIdx];
    globalTurnCounter++;
    char currentTileChar = 'a' + (roundNumber - 1) % 26;
    
    cout << "\n" << colorText("========================================", YELLOW) << "\n";
    cout << colorText("Tour " + to_string(roundNumber), YELLOW) << " - " << player.getColoredName() 
         << " (Coupons: " << colorText(to_string(player.getSwapCoupons()), PURPLE) 
         << ", Tuile: " << colorText(string(1, currentTileChar), player.getColor()) << ")\n";
    cout << colorText("========================================", YELLOW) << "\n";
    
    cout << colorText("Note: ", YELLOW) << colorText("#", RED) << " = case de reference pour le placement de tuile\n";
    
    board.display(players);
    
    if (currentTileIndex >= tileQueue.size()) {
        cout << "Plus de tuiles disponibles!\n";
        return;
    }
    
    GameTile currentTile = tileQueue[currentTileIndex];
    
    cout << "\n" << colorText("Tuile actuelle:", YELLOW) << "\n";
    currentTile.displayPreview();
    
    currentTileIndex++;
    
    bool placed = false;
    while (!placed) {
        cout << "\n" << colorText("Options:", CYAN) << "\n";
        cout << colorText("R", GREEN) << " - Rotation 90\n";
        cout << colorText("F", GREEN) << " - Retourner (Flip)\n";
        cout << colorText("E", GREEN) << " - Echanger tuile (Coupons: " << player.getSwapCoupons() << ")\n";
        cout << colorText("A", GREEN) << " - Afficher les 5 prochaines tuiles\n";
        cout << colorText("P", GREEN) << " - Placer la tuile\n";
        cout << "\nVotre choix: ";
        
        char choice;
        cin >> choice;
        choice = toupper(choice);
        
        switch (choice) {
            case 'R':
                currentTile.rotate90();
                cout << colorText("Tuile tournee de 90!", GREEN) << "\n\n";
                board.display(players);
                cout << "\nTuile actuelle:\n";
                currentTile.displayPreview();
                break;
                
            case 'F':
                currentTile.flip();
                cout << colorText("Tuile retournee!", GREEN) << "\n\n";
                board.display(players);
                cout << "\nTuile actuelle:\n";
                currentTile.displayPreview();
                break;
                
            case 'E': {
                if (player.getSwapCoupons() > 0) {
                    cout << "\n" << colorText("=== ECHANGE DE TUILE ===", PURPLE) << "\n";
                    cout << "Choisissez parmi les 5 prochaines tuiles:\n\n";
                    
                    vector<int> availableChoices;
                    for (int i = 1; i <= 5 && currentTileIndex - 1 + i < (int)tileQueue.size(); i++) {
                        cout << colorText("Choix " + to_string(i) + ":", GREEN) << "\n";
                        tileQueue[currentTileIndex - 1 + i].displayPreview();
                        cout << "\n";
                        availableChoices.push_back(i);
                    }
                    
                    if (!availableChoices.empty()) {
                        int choice;
                        cout << "Votre choix (1-" << availableChoices.size() << "): ";
                        cin >> choice;
                        if (choice >= 1 && choice <= (int)availableChoices.size() && 
                            currentTileIndex - 1 + choice < (int)tileQueue.size()) {
                            if (player.useSwapCoupon()) {
                                currentTile = tileQueue[currentTileIndex - 1 + choice];
                                tileQueue.erase(tileQueue.begin() + currentTileIndex - 1 + choice);
                                cout << colorText("Tuile echangee !", GREEN) << "\n\n";
                                board.display(players);
                                cout << "\nVotre nouvelle tuile:\n";
                                currentTile.displayPreview();
                            }
                        } else {
                            cout << colorText("Choix invalide!", RED) << "\n";
                        }
                    } else {
                        cout << colorText("Aucune tuile disponible pour l'echange!", RED) << "\n";
                    }
                } else {
                    cout << colorText("Pas de coupon d'echange disponible!", RED) << "\n";
                }
                break;
            }
                
            case 'A':
                cout << "\n" << colorText("=== PROCHAINES TUILES ===", CYAN) << "\n";
                for (int i = 1; i <= 5 && currentTileIndex - 1 + i < (int)tileQueue.size(); i++) {
                    cout << colorText("Tuile +" + to_string(i) + ":", YELLOW) << "\n";
                    tileQueue[currentTileIndex - 1 + i].displayPreview();
                    cout << "\n";
                }
                if (currentTileIndex - 1 + 1 >= (int)tileQueue.size()) {
                    cout << colorText("Aucune tuile suivante disponible.", YELLOW) << "\n";
                }
                break;
                
            case 'P': {
                cout << "Position (ex: J10): ";
                string posInput;
                cin >> posInput;
                Position testPos = parseCoordinates(posInput);
                
                if (testPos.x == -1 || testPos.y == -1) {
                    cout << colorText("Format invalide! Utilisez le format 'LettreNumero' (ex: J10)", RED) << "\n";
                    break;
                }
                
                vector<Position> positions = currentTile.getAbsolutePositions(testPos);
                
                bool valid = true;
                bool touchesTerritory = false;
                
                for (const auto& pos : positions) {
                    if (!board.isValidPosition(pos)) {
                        cout << colorText("Position hors du plateau!", RED) << "\n";
                        valid = false;
                        break;
                    }
                    if (!board.isEmpty(pos)) {
                        cout << colorText("Position deja occupee!", RED) << "\n";
                        valid = false;
                        break;
                    }
                }
                
                if (valid) {
                    for (int p = 0; p < positions.size(); p++) {
                        Position pos = positions[p];
                        vector<Position> adjacent;
                        adjacent.push_back(Position(pos.x-1, pos.y));
                        adjacent.push_back(Position(pos.x+1, pos.y));
                        adjacent.push_back(Position(pos.x, pos.y-1));
                        adjacent.push_back(Position(pos.x, pos.y+1));
                        
                        for (int a = 0; a < adjacent.size(); a++) {
                            Position adj = adjacent[a];
                            if (player.hasPosition(adj)) {
                                touchesTerritory = true;
                                break;
                            }
                            for (int i = 0; i < players.size(); i++) {
                                if (players[i].hasPosition(adj)) {
                                    cout << colorText("Ne peut pas toucher le territoire adverse!", RED) << "\n";
                                    valid = false;
                                    break;
                                }
                            }
                            if (!valid) break;
                        }
                        if (!valid || touchesTerritory) break;
                    }
                    
                    if (valid && !touchesTerritory) {
                        cout << colorText("Doit toucher au moins une case de votre territoire!", RED) << "\n";
                        valid = false;
                    }
                }
                
                if (valid && touchesTerritory) {
                    board.placeTile(positions, playerIdx, currentTileChar);
                    player.addToTerritory(positions);
                    
                    auto captured = board.checkBonusCapture(positions, playerIdx);
                    for (int b = 0; b < captured.size(); b++) {
                        Bonus* bonus = captured[b];
                        if (bonus->type == TILE_SWAP) {
                            player.addSwapCoupon();
                            cout << colorText("Bonus: Coupon d'echange obtenu!", GREEN) << "\n";
                        } else if (bonus->type == STONE) {
                            cout << colorText("Bonus Pierre: Placer une pierre (ex: J10): ", YELLOW);
                            string stoneInput;
                            cin >> stoneInput;
                            Position stonePos = parseCoordinates(stoneInput);
                            if (stonePos.x != -1 && stonePos.y != -1 && 
                                board.isValidPosition(stonePos) && board.isEmpty(stonePos)) {
                                board.placeStone(stonePos);
                                cout << colorText("Pierre placee!", GREEN) << "\n";
                            } else {
                                cout << colorText("Position invalide pour la pierre!", RED) << "\n";
                            }
                        } else if (bonus->type == STEAL) {
                            cout << colorText("Bonus Vol: Choisir un joueur a voler (1-" + to_string(players.size()) + "): ", YELLOW);
                            int targetIdx;
                            cin >> targetIdx;
                            targetIdx--;
                            
                            if (targetIdx >= 0 && targetIdx < (int)players.size() && targetIdx != playerIdx) {
                                cout << colorText("Lettre de la tuile a voler (ex: a, b, c...): ", YELLOW);
                                char tileChar;
                                cin >> tileChar;
                                tileChar = tolower(tileChar);
                                
                                vector<Position> tilePositions;
                                for (const auto& pos : players[targetIdx].getTerritory()) {
                                    if (board.getTileChar(pos) == tileChar) {
                                        tilePositions.push_back(pos);
                                    }
                                }
                                
                                if (!tilePositions.empty()) {
                                    cout << colorText("Tuile '" + string(1, tileChar) + "' de " + to_string(tilePositions.size()) + " cases trouvee!", CYAN) << "\n";
                                    cout << colorText("Ou voulez-vous replacer cette tuile? (ex: J10): ", YELLOW);
                                    string newPosInput;
                                    cin >> newPosInput;
                                    Position newBasePos = parseCoordinates(newPosInput);
                                    
                                    if (newBasePos.x != -1 && newBasePos.y != -1) {
                                        Position offset = Position(newBasePos.x - tilePositions[0].x, 
                                                                 newBasePos.y - tilePositions[0].y);
                                        
                                        vector<Position> newPositions;
                                        bool validPlacement = true;
                                        
                                        for (const auto& pos : tilePositions) {
                                            Position newPos(pos.x + offset.x, pos.y + offset.y);
                                            if (!board.isValidPosition(newPos) || !board.isEmpty(newPos)) {
                                                validPlacement = false;
                                                break;
                                            }
                                            newPositions.push_back(newPos);
                                        }
                                        
                                        if (validPlacement) {
                                            for (const auto& pos : tilePositions) {
                                                players[targetIdx].removeFromTerritory(pos);
                                                board.removeTile(pos);
                                            }
                                            
                                            player.addToTerritory(newPositions);
                                            for (const auto& pos : newPositions) {
                                                board.placeTile({pos}, playerIdx, tileChar);
                                            }
                                            
                                            cout << colorText("Tuile '" + string(1, tileChar) + "' volee et replacee!", GREEN) << "\n";
                                        } else {
                                            cout << colorText("Impossible de placer la tuile volee a cette position!", RED) << "\n";
                                        }
                                    } else {
                                        cout << colorText("Position invalide pour le replacement!", RED) << "\n";
                                    }
                                } else {
                                    cout << colorText("Aucune tuile avec la lettre '" + string(1, tileChar) + "' trouvee chez ce joueur!", RED) << "\n";
                                }
                            } else {
                                cout << colorText("Joueur invalide!", RED) << "\n";
                            }
                        }
                    }
                    
                    cout << colorText("Tuile placee avec succes!", GREEN) << "\n";
                    board.display(players);
                    return;
                } else {
                    cout << colorText("Placement invalide!", RED) << "\n";
                }
                break;
            }
                
            default:
                cout << colorText("Choix invalide! Utilisez R, F, E, A ou P", RED) << "\n";
                break;
        }
    }
}

void Game::play() {
    initialize();
    
    for (int round = 0; round < MAX_ROUNDS; round++) {
        roundNumber = round + 1;
        for (int i = 0; i < players.size(); i++) {
            playTurn(i);
        }
    }
    
    cout << "\n=== PHASE FINALE: Tuiles 1x1 ===\n";
    for (int p = 0; p < players.size(); p++) {
        Player* player = &players[p];
        if (player->useSwapCoupon()) {
            cout << player->getName() << " peut placer une tuile 1x1.\n";
            board.display(players);
            cout << "Position (ex: J10): ";
            string input;
            cin >> input;
            Position pos = parseCoordinates(input);
            
            if (pos.x != -1 && pos.y != -1 && 
                board.isValidPosition(pos) && board.isEmpty(pos)) {
                board.placeTile({pos}, p, '*');
                player->addToTerritory({pos});
            } else {
                cout << "Position invalide!\n";
            }
        }
    }
    
    cout << "\n=== RESULTATS FINAUX ===\n";
    int maxSquare = 0;
    int winnerId = 0;
    
    for (int i = 0; i < players.size(); i++) {
        int square = players[i].getLargestSquare();
        int territory = players[i].getTerritory().size();
        
        cout << players[i].getName() << ": " << square << "x" << square 
             << " (territoire: " << territory << " cases)\n";
        
        if (square > maxSquare) {
            maxSquare = square;
            winnerId = i;
        } else if (square == maxSquare && territory > players[winnerId].getTerritory().size()) {
            winnerId = i;
        }
    }
    
    cout << "\n*** VAINQUEUR: " << players[winnerId].getName() << " ***\n";
    board.display(players);
}