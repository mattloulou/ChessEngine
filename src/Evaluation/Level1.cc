#include "Level1.h"
#include <cmath>

EvalScore Level1::computeEval(const BitBoard& board, const Player botPlayer)
{
    const BoardStateSimplified state = board.getStateSimplified();
    return EvalScore{rand() % 100, state.whiteHasWin || state.hasDraw || state.blackHasWin};
}
