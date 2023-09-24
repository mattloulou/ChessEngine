#ifndef LEVEL4BASE_H
#define LEVEL4BASE_H

#include "Decorator.h"

class Level4Base : public Evaluator {
public:
    EvalScore computeEval(const BitBoard& board, const Player botPlayer) override;
};

#endif
