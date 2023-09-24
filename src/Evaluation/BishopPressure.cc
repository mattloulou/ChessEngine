#include "BishopPressure.h"

BishopPressure::BishopPressure(std::unique_ptr<Evaluator> next): Decorator{std::move(next)} {}

EvalScore BishopPressure::computeEval(const BitBoard& board, Player botPlayer)
{
    int score = next->computeEval(board, botPlayer).evaluation;
    
    score += board.bishopPressureBonus(Player::White);
    score += -1 * board.bishopPressureBonus(Player::Black);
    return EvalScore{score, false};
}
