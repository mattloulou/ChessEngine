#include "Level4Base.h"

EvalScore Level4Base::computeEval(const BitBoard& board, const Player botPlayer)
{
    int score = 0;

    // calculate the scores from each player
    score += board.getPieceScores(Player::White);
    score += -1 * board.getPieceScores(Player::Black);

    // add slight randomness for games to differ
    score += rand()%6;

    return EvalScore{score, false};
}
