#include "PushPawnsLateGame.h"

PushPawnsLateGame::PushPawnsLateGame(std::unique_ptr<Evaluator> next): Decorator{std::move(next)} {}

EvalScore PushPawnsLateGame::computeEval(const BitBoard& board, Player botPlayer)
{
    int score = next->computeEval(board, botPlayer).evaluation;
    
    score += board.pushedPawnsLateGame(Player::White);
    score += -1 * board.pushedPawnsLateGame(Player::Black);
    return EvalScore{score, false};
}
