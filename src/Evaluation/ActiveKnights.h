#ifndef ACTIVEKNIGHTS_H
#define ACTIVEKNIGHTS_H

#include "Decorator.h"

class ActiveKnights : public Decorator {
public:
    ActiveKnights(std::unique_ptr<Evaluator> next);
    EvalScore computeEval(const BitBoard& board, const Player botPlayer) override;
};

#endif
