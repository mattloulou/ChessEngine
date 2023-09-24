#ifndef BOARDSTATE_H
#define BOARDSTATE_H

#include <vector>
#include "Player.h"
#include "Piece.h"
#include "BoardStateSimplified.h"

struct BoardState : public BoardStateSimplified {
    
    // all the pieces
    std::vector<Piece> grid;

    Player playerTurn;
    
    // if either player is in check
    bool whiteInCheck;
    bool blackInCheck;

    BoardState(std::vector<Piece> grid, Player playerTurn, bool whiteHasWin, bool hasDraw, bool blackHasWin, bool whiteInCheck, bool blackInCheck);
    
};

#endif
