#ifndef BOARDSTATESIMPLIFIED_H
#define BOARDSTATESIMPLIFIED_H

#include <vector>
#include "Player.h"
#include "Piece.h"

struct BoardStateSimplified {

    // info about game ending
    bool whiteHasWin;
    bool hasDraw;
    bool blackHasWin;

    BoardStateSimplified(bool whiteHasWin, bool hasDraw, bool blackHasWin);
    
};

#endif
