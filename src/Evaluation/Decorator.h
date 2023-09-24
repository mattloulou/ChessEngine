#ifndef DECORATOR_H
#define DECORATOR_H

#include "Evaluator.h"
#include "EvalScore.h"
#include <memory>

class Decorator : public Evaluator {
protected:
    std::unique_ptr<Evaluator> next;
public:
    Decorator(std::unique_ptr<Evaluator> next);
    ~Decorator() override {}
};

#endif
