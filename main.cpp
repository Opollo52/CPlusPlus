#include <iostream>
#include <vector>
#include "colors.h"
#include "game.h"
#include "player.h"

using namespace std;

int main() {
    enableColors();
    
    cout << colorText("=== LAYING GRASS ===\n\n", YELLOW);
    
    int numPlayers;
    cout << "Nombre de joueurs (2-9): ";
    cin >> numPlayers;
    
    if (numPlayers < 2 || numPlayers > 9) {
        cout << "Nombre invalide!\n";
        return 1;
    }
    
    Game game(numPlayers);
    
    vector<PlayerColor> availableColors = {RED, BLUE, GREEN, YELLOW, ORANGE, PURPLE, PINK, BROWN, GRAY};
    
    for (int i = 0; i < numPlayers; i++) {
        string name;
        cout << "\nJoueur " << (i+1) << " - Nom: ";
        cin >> name;
        
        cout << "Couleurs disponibles:\n";
        for (size_t j = 0; j < availableColors.size(); j++) {
            cout << (j+1) << ". " << colorText(getColorName(availableColors[j]), availableColors[j]) << "\n";
        }
        
        int colorChoice;
        cout << "Choisissez une couleur (1-" << availableColors.size() << "): ";
        cin >> colorChoice;
        
        if (colorChoice < 1 || colorChoice > (int)availableColors.size()) {
            cout << "Choix invalide, couleur par defaut assignee.\n";
            colorChoice = 1;
        }
        
        PlayerColor selectedColor = availableColors[colorChoice - 1];
        availableColors.erase(availableColors.begin() + colorChoice - 1);
        
        game.addPlayer(Player(name, selectedColor, i));
    }
    
    game.play();
    
    return 0;
}