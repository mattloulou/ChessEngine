#ifndef QUEENPRESSURE_H
#define QUEENPRESSURE_H

#include "Decorator.h"

class QueenPressure : public Decorator {
public:
    QueenPressure(std::unique_ptr<Evaluator>next);
    EvalScore computeEval(const BitBoard& board, const Player botPlayer) override;
};

#endif
