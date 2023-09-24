#ifndef CONNECTEDROOKS_H
#define CONNECTEDROOKS_H

#include "Decorator.h"

class ConnectedRooks : public Decorator {
public:
    ConnectedRooks(std::unique_ptr<Evaluator> next);
    EvalScore computeEval(const BitBoard& board, const Player botPlayer) override;
};

#endif
