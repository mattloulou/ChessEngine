#ifndef EARLYGAMEKINGPOSITION_H
#define EARLYGAMEKINGPOSITION_H

#include "Decorator.h"

class EarlyGameKingPosition : public Decorator {
public:
    EarlyGameKingPosition(std::unique_ptr<Evaluator> next);
    EvalScore computeEval(const BitBoard& board, const Player botPlayer) override;
};

#endif
