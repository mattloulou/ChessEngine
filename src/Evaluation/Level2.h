#ifndef LEVEL2_H
#define LEVEL2_H

#include "Decorator.h"

class Level2 : public Evaluator {
public:
    EvalScore computeEval(const BitBoard& board, const Player botPlayer) override;
};

#endif
