#ifndef PAWNRELATIONS_H
#define PAWNRELATIONS_H

#include "Decorator.h"

class PawnRelations : public Decorator {
public:
    PawnRelations(std::unique_ptr<Evaluator>next);
    EvalScore computeEval(const BitBoard& board, const Player botPlayer) override;
};

#endif
