#include "QueenPressure.h"

QueenPressure::QueenPressure(std::unique_ptr<Evaluator> next): Decorator{std::move(next)} {}

EvalScore QueenPressure::computeEval(const BitBoard& board, Player botPlayer)
{
    int score = next->computeEval(board, botPlayer).evaluation;
    
    score += board.queenPressureBonus(Player::White);
    score += -1 * board.queenPressureBonus(Player::Black);
    return EvalScore{score, false};
}
