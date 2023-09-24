#ifndef LEVEL3_H
#define LEVEL3_H

#include "Decorator.h"

class Level3 : public Decorator {
public:
    Level3(std::unique_ptr<Evaluator>next);
    EvalScore computeEval(const BitBoard& board, const Player botPlayer) override;
};

#endif
