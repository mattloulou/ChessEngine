#ifndef PUSHPAWNSLATEGAME_H
#define PUSHPAWNSLATEGAME_H

#include "Decorator.h"

class PushPawnsLateGame : public Decorator {
public:
    PushPawnsLateGame(std::unique_ptr<Evaluator>next);
    EvalScore computeEval(const BitBoard& board, const Player botPlayer) override;
};

#endif
