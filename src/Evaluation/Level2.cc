#include "Level2.h"
#include "../BoardState.h"

EvalScore Level2::computeEval(const BitBoard& board, const Player botPlayer)
{
    const BoardState state = board.getState();
    int score = 0;
    
    if (botPlayer == Player::White) {
        score = -1 * board.getPieceScores(Player::Black);
        if (state.blackInCheck) {
            score += 300;
        }
    } else {
        score = board.getPieceScores(Player::White);
        if (state.whiteInCheck) {
            score += 300;
        }
    }
    return EvalScore{score, state.whiteHasWin || state.hasDraw || state.blackHasWin};
}
