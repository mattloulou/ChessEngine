#ifndef BISHOPPRESSURE_H
#define BISHIPPRESSURE_H

#include "Decorator.h"

class BishopPressure : public Decorator {
public:
    BishopPressure(std::unique_ptr<Evaluator> next);
    EvalScore computeEval(const BitBoard& board, const Player botPlayer) override;
};

#endif
