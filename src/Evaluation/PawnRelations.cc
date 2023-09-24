#include "PawnRelations.h"

PawnRelations::PawnRelations(std::unique_ptr<Evaluator> next): Decorator{std::move(next)} {}

EvalScore PawnRelations::computeEval(const BitBoard& board, Player botPlayer)
{
    int score = next->computeEval(board, botPlayer).evaluation;
    
    score += board.connectedAndStackedPawnBonus(Player::White);
    score += -1 * board.connectedAndStackedPawnBonus(Player::Black);
    return EvalScore{score, false};
}
