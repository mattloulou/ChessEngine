#include "BoardState.h"

BoardState::BoardState(std::vector<Piece> grid, Player playerTurn, bool whiteHasWin, bool hasDraw, bool blackHasWin, bool whiteInCheck, bool blackInCheck) :
    BoardStateSimplified{whiteHasWin, hasDraw, blackHasWin}, grid{grid}, playerTurn{playerTurn}, whiteInCheck{whiteInCheck}, blackInCheck{blackInCheck} {}
