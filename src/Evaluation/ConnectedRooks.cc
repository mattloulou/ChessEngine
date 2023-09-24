#include "ConnectedRooks.h"

ConnectedRooks::ConnectedRooks(std::unique_ptr<Evaluator> next): Decorator{std::move(next)} {}

EvalScore ConnectedRooks::computeEval(const BitBoard& board, Player botPlayer)
{
    int score = next->computeEval(board, botPlayer).evaluation;
    
    score += board.connectedRooksBonus(Player::White);
    score += -1 * board.connectedRooksBonus(Player::Black);
    return EvalScore{score, false};
}
