#ifndef LEVEL1_H
#define LEVEL1_H

#include "Evaluator.h"

class Level1 : public Evaluator {
public:
    EvalScore computeEval(const BitBoard& board, const Player botPlayer) override;
};

#endif
