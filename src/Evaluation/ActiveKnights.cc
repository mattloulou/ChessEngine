#include "ActiveKnights.h"

ActiveKnights::ActiveKnights(std::unique_ptr<Evaluator> next): Decorator{std::move(next)} {}

EvalScore ActiveKnights::computeEval(const BitBoard& board, Player botPlayer)
{
    int score = next->computeEval(board, botPlayer).evaluation;
    
    score += board.activeKnights(Player::White);
    score += -1 * board.activeKnights(Player::Black);
    return EvalScore{score, false};
}
