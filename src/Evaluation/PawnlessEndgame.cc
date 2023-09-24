#include "PawnlessEndgame.h"

PawnlessEndgame::PawnlessEndgame(std::unique_ptr<Evaluator> next): Decorator{std::move(next)} {}

EvalScore PawnlessEndgame::computeEval(const BitBoard& board, Player botPlayer)
{
    int score = next->computeEval(board, botPlayer).evaluation;
    
    score += board.mopupEvaluationBonus(Player::White);
    score += -1 * board.mopupEvaluationBonus(Player::Black);
    return EvalScore{score, false};
}
