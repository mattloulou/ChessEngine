#ifndef MASKS_H
#define MASKS_H

#include <iostream>
#include <cstdint>
#include <vector>
#include <memory>

using namespace std;
using U64 = uint64_t;

/*
 * Masks is a singleton class that (a shared_ptr can be obtained from the method `getMasks()`) 
 * computes a bunch of useful masks for computation the first time a Masks object is created.
 * A mask is simply a 64-bit unsigned integer, where each bit position corresponds to a 
 * certain position on the chess board. The LSB is the bottom left, the 7th index is the bottom right,
 * and the MSB is the top right of the board.
 */

class Masks {
private:
    // big-5 should be private and/or deleted to enforce the singleton design patter
    Masks();
    Masks(const Masks& m) = delete;

    static std::vector<U64> generateFileMasks();
    static std::vector<U64> generateRankMasks();
    static std::vector<U64> flipMasks(std::vector<U64> masks);
    static std::vector<U64> generateRookAttackMasks();
    static std::vector<U64> generateBishopAttackMasks();
    static std::vector<U64> generateQueenAttackMasks();
    static std::vector<U64> generateKnightAttackMasks();    
    static std::vector<U64> generateKingAttackMasks();   
    static std::vector<U64> generateThreeByThreeSquareMasks();     
    static std::vector<U64> generateWhitePawnMoveMasks();    
    static std::vector<U64> generateBlackPawnMoveMasks();
    static std::vector<U64> generateWhitePawnAttackMasks();
    static std::vector<U64> generateBlackPawnAttackMasks();

    static std::vector<std::vector<U64>> generateAttackRay(const int fileDelta, const int rankDelta);

public:
    const std::vector<U64> fileMasks;
    const std::vector<U64> fileNotMasks;
    const std::vector<U64> rankMasks;
    const std::vector<U64> rankNotMasks;
    const std::vector<U64> rookAttackMasks;
    const std::vector<U64> bishopAttackMasks;
    const std::vector<U64> queenAttackMasks;
    const std::vector<U64> knightAttackMasks;
    const std::vector<U64> kingAttackMasks;
    const std::vector<U64> threeByThreeSquareMask;
    const std::vector<U64> whitePawnAttackMasks;
    const std::vector<U64> whitePawnMoveMasks;
    const std::vector<U64> blackPawnAttackMasks;
    const std::vector<U64> blackPawnMoveMasks;


    // 10101010
    // 01010101
    // ....
    // 01010101
    static const U64 checkerboardPattern = 0x55AA55AA55AA55AAull;

    // 00000000
    // 00000000
    // 00111100
    // 00111100
    // 00111100
    // 00111100
    // 00000000
    // 00000000

    static const int knightPositionBonus[2][64];
    static const int bishopPositionBonus[2][64];
    static const int rookPositionBonus[2][64];
    static const int queenPositionBonus[2][64];
    static const int centreManhattanDistance[64];
    static const int enemyKingPositionWithWhiteBishop[64];
    static const int enemyKingPositionWithBlackBishop[64];
    static const int whiteKingEarlyGame[64];
    static const int blackKingEarlyGame[64];

    std::vector<std::vector<U64>> attackRayRight;
    std::vector<std::vector<U64>> attackRayDiagonalUpRight;
    std::vector<std::vector<U64>> attackRayUp;
    std::vector<std::vector<U64>> attackRayDiagonalUpLeft;
    std::vector<std::vector<U64>> attackRayLeft;
    std::vector<std::vector<U64>> attackRayDiagonalDownLeft;
    std::vector<std::vector<U64>> attackRayDown;
    std::vector<std::vector<U64>> attackRayDiagonalDownRight;

    static shared_ptr<Masks> getMasks();

    static void printMask(const U64 mask);
};

#endif
