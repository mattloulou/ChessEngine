#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "../BitBoard.h"
#include "EvalScore.h"

class Evaluator {
public:
    virtual EvalScore computeEval(const BitBoard& board, const Player botPlayer) = 0;
    virtual ~Evaluator() {}
};

#endif
