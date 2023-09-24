#include "GameOver.h"

GameOver::GameOver(std::unique_ptr<Evaluator> next): Decorator{std::move(next)} {}

EvalScore GameOver::computeEval(const BitBoard& board, Player botPlayer)
{
    const BoardStateSimplified state = board.getStateSimplified();
    int score = 0;
    
    // give a very high, or exactly 0 eval, if the game is over.
    if (state.whiteHasWin || state.hasDraw || state.blackHasWin) {
        if (state.whiteHasWin) {
            score = 10000000; // 10M
        } else if (state.blackHasWin) {
            score = -10000000; // -10M
        }
        return EvalScore{score, true};
    } else {
        score = next->computeEval(board, botPlayer).evaluation;
        return EvalScore{score, false};
    }

}
