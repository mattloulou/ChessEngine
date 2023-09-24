#ifndef COMPUTER_H
#define COMPUTER_H

#include "Move.h"
#include "BitBoard.h"
#include "./Evaluation/Evaluator.h"
#include "Player.h"
#include "Hash.h"
#include <vector>
#include <stack>

class Computer {
    // do not change the order of member variables
    // Stores the player that this computer is
    const Player player;
    // Stores the level (1-4) that this computer will play at
    const int level;
    // Stores the evaluator used for numerically evaluating board states (decorated based on the level)
    const std::unique_ptr<Evaluator> evaluator;

    // stores the depth of our search tree for the minimax algorithm for determining the best move
    const int searchDepth = 4;
    // a feature flag for turning alpha beta pruning (search optimization) on or off
    const bool pruneMode = true;
    // used for keeping track of the best option for the max player stacked for every level so the min player does not do unnecessary searching
    std::stack<int> alphaMax;

    // an instance of our hash class for storing computed evaluations
    Hash hash;

    // method to obtain all hash keys for depths greater than equal to a given depth (these are considered as good evaluations at a given board depth)
    std::vector<U64> candidateHashs(const BitBoard& curr, const int depth) const;

    // given an evaluation in terms of the white player, this returns the corresponding evaluation for the given player (negated iff black)
    int getAdjustedScore(const int score, const Player player) const;

    /*
        The below methods all relate to our implementation of the minimax algorithm for determining the best move.
        There idea is that we evaluate a state based on what the other player can do assuming they make the best
        worst move for you (the move that puts you in the worst position and them in the best). This is recursively defined
        with depth searchDepth. Once we hit the end of our depth, we define the evaluatio of a board as positive what our evaluator
        returns for the computer player and negative for the opponent player.
    */
    // returns the board evaluation without considering min/max player or depth (just for level 1-3 where we do not look ahead beyond one move)
    int basicEvalScore(const BitBoard& board) const;
    // returns the max evaluation move for the max player (the computer player) based on recursive calls to the min player or a base case that gets the +ve board evaluation
    int maxPlayer(const BitBoard& curr, int depthRemaining);
    // returns the min evaluation move for the min player (the other player) based on recursive calls to 
    int minPlayer(const BitBoard& curr, int depthRemaining);
    // returns the possible next moves for a given board and depth (the depth determines whose turn it is which determines the moves)
    std::vector<std::shared_ptr<BitBoard>> getMoves(const BitBoard& board) const;

    // sets the evaluator based on the level
    std::unique_ptr<Evaluator> setEvaluator() const;
public:
    Computer(Player player, int level);
    Computer(const Computer& other);
    // returns the best move as the new BitBoard after the move based on the current board
    std::shared_ptr<BitBoard> findMove(const BitBoard& board);
};

#endif
