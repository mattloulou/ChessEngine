#ifndef PAWNLESSENDGAME_H
#define PAWNLESSENDGAME_H

#include "Decorator.h"

class PawnlessEndgame : public Decorator {
public:
    PawnlessEndgame(std::unique_ptr<Evaluator>next);
    EvalScore computeEval(const BitBoard& board, const Player botPlayer) override;
};

#endif
