#include "colors.h"

std::string getColorCode(PlayerColor color) {
    switch (color) {
        case RED: return "\033[91m";
        case BLUE: return "\033[94m";
        case GREEN: return "\033[92m";
        case YELLOW: return "\033[93m";
        case ORANGE: return "\033[33m";
        case PURPLE: return "\033[95m";
        case PINK: return "\033[35m";
        case BROWN: return "\033[33m";
        case GRAY: return "\033[90m";
        case CYAN: return "\033[96m";
        default: return "\033[37m";
    }
}

std::string getColorName(PlayerColor color) {
    switch (color) {
        case RED: return "Rouge";
        case BLUE: return "Bleu";
        case GREEN: return "Vert";
        case YELLOW: return "Jaune";
        case ORANGE: return "Orange";
        case PURPLE: return "Violet";
        case PINK: return "Rose";
        case BROWN: return "Marron";
        case GRAY: return "Gris";
        case CYAN: return "Cyan";
        default: return "Inconnu";
    }
}

std::string getResetColor() {
    return "\033[0m";
}

std::string colorText(const std::string& text, PlayerColor color) {
    return getColorCode(color) + text + getResetColor();
}

void enableColors() {
    #ifdef _WIN32
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;
        GetConsoleMode(hOut, &dwMode);
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
    #endif
}