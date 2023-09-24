#include "Decorator.h"

Decorator::Decorator(std::unique_ptr<Evaluator> next): next{std::move(next)} {}
