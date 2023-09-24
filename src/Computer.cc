#include "Computer.h"
#include <numeric>
#include <algorithm>
#include <random>
#include "./Evaluation/Level1.h"
#include "./Evaluation/Level2.h"
#include "./Evaluation/Level3.h"
#include "./Evaluation/GameOver.h"
#include "./Evaluation/ActiveKnights.h"
#include "./Evaluation/BishopPressure.h"
#include "./Evaluation/ConnectedRooks.h"
#include "./Evaluation/EarlyGameKingPosition.h"
#include "./Evaluation/PawnlessEndgame.h"
#include "./Evaluation/PawnRelations.h"
#include "./Evaluation/PushPawnsLateGame.h"
#include "./Evaluation/QueenPressure.h"
#include "./Evaluation/RookPressure.h"
#include "./Evaluation/Level4Base.h"

std::unique_ptr<Evaluator> Computer::setEvaluator() const
{
    if (level == 1) {
        return std::make_unique<Level1>();
    } else if (level == 2) {
        return std::make_unique<Level2>();
    } else if (level == 3) {
        return std::make_unique<Level3>(std::make_unique<Level2>());
    } else {
        return std::make_unique<GameOver>(
            std::make_unique<EarlyGameKingPosition>(
                std::make_unique<ActiveKnights>(
                    std::make_unique<BishopPressure>(
                        std::make_unique<ConnectedRooks>(
                            std::make_unique<PawnlessEndgame>(
                                std::make_unique<PawnRelations>(
                                    std::make_unique<PushPawnsLateGame>(
                                        std::make_unique<QueenPressure>(
                                            std::make_unique<RookPressure>(
                                                std::make_unique<Level4Base>()
                                            )
                                        )
                                    )
                                )
                            )
                        )
                    )
                )
            )
        );
    }
}

Computer::Computer(Player player, int level): player{player}, level{level}, evaluator{setEvaluator()} {}

Computer::Computer(const Computer& other): player{other.player}, level{other.level}, evaluator{setEvaluator()} {}

std::shared_ptr<BitBoard> Computer::findMove(const BitBoard& board)
{
    std::vector<std::shared_ptr<BitBoard>> nextMoves = getMoves(board);
    int chosenMax = INT_MIN;
    if (pruneMode) {
        alphaMax.push(chosenMax);
    }
    std::shared_ptr<BitBoard> chosenBoard;

    hash.clearHashTables();
    
    // shuffle the input moves for level 2 and 3 to lead to more interesting games
    if (2 <= level && level <= 3) {
        auto rd = std::random_device{};
        auto rng = std::default_random_engine{rd()};
        std::shuffle(nextMoves.begin(), nextMoves.end(), rng);
    }

    int recursiveDepth = searchDepth;
    if (nextMoves.size() < 8) {
        ++recursiveDepth;
    } 
    if (nextMoves.size() < 3) {
        ++recursiveDepth;
    }

    for (const auto& nextMove : nextMoves) {
        
        // check the eval of this board and update as needed
        const int currValue = ((level <= 3) ? getAdjustedScore(basicEvalScore(*nextMove), player) : minPlayer(*nextMove, recursiveDepth - 1));
        if (currValue > chosenMax) {
            chosenMax = currValue;
            chosenBoard = nextMove;
            // keep our max accurate for maximal pruning
            if (pruneMode) {
                alphaMax.pop();
                alphaMax.push(chosenMax);
            }
        }
    }
    alphaMax.pop();
    return chosenBoard;
}

int Computer::getAdjustedScore(const int score, const Player player) const
{
    if (player == Player::White) {
        return score;
    } else {
        return -score;
    }
}

int Computer::basicEvalScore(const BitBoard& board) const
{
    return evaluator->computeEval(board, player).evaluation;
}

std::vector<U64> Computer::candidateHashs(const BitBoard& curr, const int depth) const
{
    // get the hashs for all the depths remaining greater than or equal the current
    std::vector<U64> options;
    for (int tryDepth = searchDepth+2; tryDepth >= depth; --tryDepth) {
        options.push_back(hash.getHashKey(curr, tryDepth));
    }
    return options;
}

int Computer::maxPlayer(const BitBoard& curr, const int depthRemaining)
{
    const std::vector<U64> options = candidateHashs(curr, depthRemaining);
    for (const auto option : options) {
        if (hash.keyExists(option, true)) {
            return hash.getValue(option, true);
        }
    }
    const EvalScore evalScore = evaluator->computeEval(curr, player);
    if (evalScore.gameOver) {
        const int adjustedScore = getAdjustedScore(evalScore.evaluation, player);

        if (adjustedScore == 0) {
            return 0;

        // increase the score if we are close to checkmate (regardless of the player, put it towards their magnitude)
        } else {
            if (adjustedScore > 0) {
                return adjustedScore + depthRemaining;
            } else {
                return adjustedScore - depthRemaining;
            }
        }
    }
    // base case: we get our adjusted board evaluation
    if (depthRemaining == 0) {
        return getAdjustedScore(evalScore.evaluation, player);
    }
    std::vector<std::shared_ptr<BitBoard>> children = getMoves(curr);
    int maxMove = INT_MIN;
    if (pruneMode) {
        alphaMax.push(INT_MIN);
    }
    for (const auto& child : children) {
        int currValue = minPlayer(*child, depthRemaining - 1);
        if (currValue > maxMove) {
            maxMove = currValue;
            if (pruneMode) {
                alphaMax.pop();
                alphaMax.push(maxMove);
            }
        }
    }
    alphaMax.pop();
    hash.setKeyValue(options[options.size() - 1], maxMove, true);
    return maxMove;
}

int Computer::minPlayer(const BitBoard& curr, const int depthRemaining)
{
    std::vector<U64> options = candidateHashs(curr, depthRemaining);
    for (auto option : options) {
        if (hash.keyExists(option, false)) {
            return hash.getValue(option, false);
        }
    }
    const EvalScore evalScore = evaluator->computeEval(curr, Player(6-player));
    if (evalScore.gameOver) {
        const int adjustedScore = -getAdjustedScore(evalScore.evaluation, Player(6-player));

        if (adjustedScore == 0) {
            return 0;

        // increase the score if we are close to checkmate (regardless of the player, put it towards their magnitude)
        } else {
            if (adjustedScore > 0) {
                return adjustedScore + depthRemaining;
            } else {
                return adjustedScore - depthRemaining;
            }
        }
    }
    // base case: we get our adjusted board evaluation, but negate it since we are the min player
    if (depthRemaining == 0) {
        int rawScore = -evaluator->computeEval(curr, Player(6-player)).evaluation;
        return getAdjustedScore(rawScore, Player(6-player));
    }
    std::vector<std::shared_ptr<BitBoard>> children = getMoves(curr);
    int minMove = INT_MAX;
    for (const auto& child : children) {
        int currValue = maxPlayer(*child, depthRemaining - 1);
        if (currValue < minMove) {
            minMove = currValue;
        }

        // stop searching if the other player can already put you in a worse spot than what you already have
        if (pruneMode && currValue < alphaMax.top()) {
            break;
        }
    }
    hash.setKeyValue(options[options.size() - 1], minMove, false);
    return minMove;
}

std::vector<std::shared_ptr<BitBoard>> Computer::getMoves(const BitBoard& board) const
{
    return board.generateValidNextPositions();
}
