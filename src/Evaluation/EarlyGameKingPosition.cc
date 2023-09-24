#include "EarlyGameKingPosition.h"

EarlyGameKingPosition::EarlyGameKingPosition(std::unique_ptr<Evaluator> next): Decorator{std::move(next)} {}

EvalScore EarlyGameKingPosition::computeEval(const BitBoard& board, Player botPlayer)
{
    int score = next->computeEval(board, botPlayer).evaluation;
    
    score += board.kingPositionEarlyGame(Player::White);
    score += -1 * board.kingPositionEarlyGame(Player::Black);
    return EvalScore{score, false};
}
