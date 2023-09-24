#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <iostream>
#include "Player.h"

class GameBoard;
class Game;
class ChessController {
    bool automatic = false;

    Player playAutomatic(Game& game, int currentTurn, bool isWhiteHuman, bool isBlackHuman);
    bool isValidPosition(const std::string position) const;
    int numberArg(const std::string& line) const;
public:
    void runController();
    void endProgram(GameBoard &board);

    // extra features
    void setAutomatic();
};

#endif
