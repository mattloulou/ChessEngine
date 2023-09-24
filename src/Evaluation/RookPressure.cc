#include "RookPressure.h"

RookPressure::RookPressure(std::unique_ptr<Evaluator> next): Decorator{std::move(next)} {}

EvalScore RookPressure::computeEval(const BitBoard& board, Player botPlayer)
{
    int score = next->computeEval(board, botPlayer).evaluation;
    
    score += board.rookPressureBonus(Player::White);
    score += -1 * board.rookPressureBonus(Player::Black);
    return EvalScore{score, false};
}
