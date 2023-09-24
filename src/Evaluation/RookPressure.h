#ifndef ROOKPRESSURE_H
#define ROOKPRESSURE_H

#include "Decorator.h"

class RookPressure : public Decorator {
public:
    RookPressure(std::unique_ptr<Evaluator>next);
    EvalScore computeEval(const BitBoard& board, const Player botPlayer) override;
};

#endif
