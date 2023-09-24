#ifndef GAMEOVER_H
#define GAMEOVER_H

#include "Decorator.h"

// should be the outer-most decorator in the chain of decorators for Level 4
class GameOver : public Decorator {
public:
    GameOver(std::unique_ptr<Evaluator>next);
    EvalScore computeEval(const BitBoard& board, const Player botPlayer) override;
};

#endif
