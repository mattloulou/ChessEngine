#include "Level3.h"

Level3::Level3(std::unique_ptr<Evaluator> next): Decorator{std::move(next)} {}

EvalScore Level3::computeEval(const BitBoard& board, Player botPlayer)
{
    const BoardStateSimplified state = board.getStateSimplified();
    int score = next->computeEval(board, botPlayer).evaluation;
    if (botPlayer == Player::White) {
        score += board.getPieceScores(Player::White);
    } else {
        score += -1 * board.getPieceScores(Player::Black);
    }
    return EvalScore{score, state.whiteHasWin || state.hasDraw || state.blackHasWin};
}
