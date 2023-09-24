#include <iostream>
#include <cstdint>
#include <vector>
#include <memory>
#include "Masks.h"

// this constructor instantiates all the masks.
Masks::Masks() : fileMasks{generateFileMasks()}, fileNotMasks{flipMasks(generateFileMasks())}, rankMasks{generateRankMasks()},
        rankNotMasks{flipMasks(generateRankMasks())}, rookAttackMasks{generateRookAttackMasks()},
        bishopAttackMasks{generateBishopAttackMasks()}, queenAttackMasks{generateQueenAttackMasks()},
        knightAttackMasks{generateKnightAttackMasks()}, kingAttackMasks{generateKingAttackMasks()}, threeByThreeSquareMask{generateThreeByThreeSquareMasks()},
        whitePawnAttackMasks{generateWhitePawnAttackMasks()}, whitePawnMoveMasks{generateWhitePawnMoveMasks()},
        blackPawnAttackMasks{generateBlackPawnAttackMasks()}, blackPawnMoveMasks{generateBlackPawnMoveMasks()},
        attackRayRight{generateAttackRay(1,0)}, attackRayDiagonalUpRight{generateAttackRay(1,1)}, 
        attackRayUp{generateAttackRay(0,1)}, attackRayDiagonalUpLeft{generateAttackRay(-1,1)}, 
        attackRayLeft{generateAttackRay(-1,0)}, attackRayDiagonalDownLeft{generateAttackRay(-1,-1)}, 
        attackRayDown{generateAttackRay(0,-1)}, attackRayDiagonalDownRight{generateAttackRay(1,-1)} {}

// this is used for a client to get access to the Masks singleton.
shared_ptr<Masks> Masks::getMasks() 
{
    static shared_ptr<Masks> masks(new Masks); // this line uses the "magic static" C++ feature to create a singleton.
                                               // this variable is instantiated only the first time the method is run.
    return masks;
}

void Masks::printMask(const U64 mask) 
{
    // prints a mask, with the LSB at the bottom left, the 8th LSB at the bottom right, and the MSB at the top right.
    for (int rank = 7; rank >= 0; --rank) {
        for (int file = 0; file < 8; ++file) {
            std::cout << ((mask & (1ull << (rank*8 + file))) ? "🟩": "🟥");
        }
        std::cout << endl;
    }
}

std::vector<U64> Masks::generateFileMasks() 
{
    U64 firstFile = 0x0101010101010101ull;
    std::vector<U64> masks = std::vector<U64>(64,0);

    // note that each rank in a file has the same mask.
    for (int file = 0; file < 8; ++file) {
        for (int rank = 0; rank < 8; ++rank) {
            masks[rank*8 + file] = firstFile;
        }
        firstFile <<= 1;
    }
    return masks;
}

std::vector<U64> Masks::generateRankMasks() 
{
    U64 firstRank = 0x00000000000000FFull;
    std::vector<U64> masks = std::vector<U64>(64,0);

    // note that each rank in a file has the same mask.
    for (int rank = 0; rank < 8; ++rank) {
        for (int file = 0; file < 8; ++file) {
            masks[rank*8+file] = firstRank;
        }
        firstRank <<= 8;
    }
    return masks;
}

std::vector<U64> Masks::flipMasks(std::vector<U64> masks) 
{
    for (auto& m : masks) {
        m = ~m;
    }
    return masks;
}

std::vector<U64> Masks::generateRookAttackMasks() 
{
    U64 firstRank = 0x00000000000000FFull;
    U64 firstFile = 0x0101010101010101ull;
    std::vector<U64> masks = std::vector<U64>(64,0);
    for (int rank = 0; rank < 8; ++rank) {
        for (int file = 0; file < 8; ++file) {

            // this sets the mask to the the combination of the correct rook and file, subtract the position where the rook is currently at.
            masks[rank*8+file] = ((firstFile << file) | (firstRank << (rank*8))) & ~(1ull << (rank*8 + file));
        }
    }
    return masks;
}

std::vector<U64> Masks::generateBishopAttackMasks() 
{
    std::vector<U64> masks = std::vector<U64>(64,0);

    // to create each bishop attack mask, I shift the main and anti diagonals a certain amount left or right, and then combine them, excluding the bishop's position.
    const U64 mainDiag = 0x0102040810204080ull;
    const U64 antiDiag = 0x8040201008040201ull;
    const U64 deleteFirstFile = ~0x0101010101010101ull;
    const U64 deleteLastFile = ~0x8080808080808080ull;
    for (int rank = 0; rank < 8; ++rank) {
        for (int file = 0; file < 8; ++file) {

            // shift the main diagonal and anti diagonals either left or right, based on the bishop's position.
            int8_t antiAmountToShiftBoardLeft = rank - file;
            int8_t mainAmountToShiftBoardLeft = 7 - (rank + file);
            U64 shiftedAntiDiag = antiDiag;
            U64 shiftedMainDiag = mainDiag;
            while (antiAmountToShiftBoardLeft > 0) {
                shiftedAntiDiag >>= 1;
                shiftedAntiDiag &= deleteLastFile; 
                --antiAmountToShiftBoardLeft;
            }
            while (antiAmountToShiftBoardLeft < 0) {
                shiftedAntiDiag <<= 1;
                shiftedAntiDiag &= deleteFirstFile; 
                ++antiAmountToShiftBoardLeft;
            }
            while (mainAmountToShiftBoardLeft > 0) {
                shiftedMainDiag >>= 1;
                shiftedMainDiag &= deleteLastFile; 
                --mainAmountToShiftBoardLeft;
            }
            while (mainAmountToShiftBoardLeft < 0) {
                shiftedMainDiag <<= 1;
                shiftedMainDiag &= deleteFirstFile; 
                ++mainAmountToShiftBoardLeft;
            }

            // set the mask for the current position to the combination of the two shifted diagonals, excluding the current position.
            masks[rank*8 + file] = (shiftedAntiDiag | shiftedMainDiag) & ~(1ull << (rank*8 + file));
        }
    }
    return masks;
}

std::vector<U64> Masks::generateQueenAttackMasks() 
{
    // a queen's attack mask is just the combination of the rook's and bishop's masks at a given position.

    std::vector<U64> rookMasks = generateRookAttackMasks();
    std::vector<U64> bishopMasks = generateBishopAttackMasks();
    std::vector<U64> masks = std::vector<U64>(64,0);
    for (int i = 0; i < 64; ++i) {
        masks[i] = rookMasks[i] | bishopMasks[i];
    }
    return masks;
}

std::vector<U64> Masks::generateKnightAttackMasks() 
{
    std::vector<U64> masks = std::vector<U64>(64,0);
    const U64 deleteFirstFile = ~0x0101010101010101ull;
    const U64 deleteLastFile = ~0x8080808080808080ull;
    const U64 deleteFirstRank = ~0x00000000000000FFull;
    const U64 deleteLastRank = ~0xFF00000000000000ull;

    // start with the mask for the knight at file 3 rank 3
    const U64 defaultKnightMask = 0x0000142200221400ull;

    // then, shift the knight mask to the appropriate position.
    for (int rank = 0; rank < 8; ++rank) {
        for (int file = 0; file < 8; ++file) {

            int8_t amountToShiftBoardUp = rank - 3;
            int8_t amountToShiftBoardRight = file - 3;
            U64 shiftedBoard = defaultKnightMask;
            while (amountToShiftBoardUp > 0) {
                shiftedBoard <<= 8;
                shiftedBoard &= deleteFirstRank;
                --amountToShiftBoardUp;
            }
            while (amountToShiftBoardUp < 0) {
                shiftedBoard >>= 8;
                shiftedBoard &= deleteLastRank;
                ++amountToShiftBoardUp;
            }
            while (amountToShiftBoardRight > 0) {
                shiftedBoard <<= 1;
                shiftedBoard &= deleteFirstFile;
                --amountToShiftBoardRight;
            }
            while (amountToShiftBoardRight < 0) {
                shiftedBoard >>= 1;
                shiftedBoard &= deleteLastFile;
                ++amountToShiftBoardRight;
            }

            masks[rank*8 + file] = shiftedBoard;
        }
    }
    return masks;
}

std::vector<U64> Masks::generateKingAttackMasks() 
{
    std::vector<U64> masks = std::vector<U64>(64,0);
    const U64 deleteFirstFile = ~0x0101010101010101ull;
    const U64 deleteLastFile = ~0x8080808080808080ull;
    const U64 deleteFirstRank = ~0x00000000000000FFull;
    const U64 deleteLastRank = ~0xFF00000000000000ull;

    // start with the mask for the king at file 3 rank 3
    const U64 defaultKingMask = 0x0000001C141C0000ull;

    // then, shift the king mask to the appropriate position.
    for (int rank = 0; rank < 8; ++rank) {
        for (int file = 0; file < 8; ++file) {

            int8_t amountToShiftBoardUp = rank - 3;
            int8_t amountToShiftBoardRight = file - 3;
            U64 shiftedBoard = defaultKingMask;
            while (amountToShiftBoardUp > 0) {
                shiftedBoard <<= 8;
                shiftedBoard &= deleteFirstRank;
                --amountToShiftBoardUp;
            }
            while (amountToShiftBoardUp < 0) {
                shiftedBoard >>= 8;
                shiftedBoard &= deleteLastRank;
                ++amountToShiftBoardUp;
            }
            while (amountToShiftBoardRight > 0) {
                shiftedBoard <<= 1;
                shiftedBoard &= deleteFirstFile;
                --amountToShiftBoardRight;
            }
            while (amountToShiftBoardRight < 0) {
                shiftedBoard >>= 1;
                shiftedBoard &= deleteLastFile;
                ++amountToShiftBoardRight;
            }

            masks[rank*8 + file] = shiftedBoard;
        }
    }
    return masks;
}

std::vector<U64> Masks::generateThreeByThreeSquareMasks() 
{
    std::vector<U64> masks = generateKingAttackMasks();
    for (int rank = 0; rank < 8; ++rank) {
        for (int file = 0; file < 8; ++file) {
            masks[rank*8+file] |= (1ull << (rank*8 + file));
        }
    }

    return masks;
}

std::vector<U64> Masks::generateWhitePawnMoveMasks() 
{
    std::vector<U64> masks = std::vector<U64>(64,0);
    
    // pawns can't go on the first or last ranks
    for (int rank = 1; rank < 7; ++rank) {
        for (int file = 0; file < 8; ++file) {
            masks[rank*8 + file] = (1ull << ((rank+1)*8 + file));

            // if a white pawn is on the second rank, it can go a second space forward; not just once.
            if (rank == 1) {
                masks[rank*8 + file] |= (1ull << ((rank+2)*8 + file));
            }
        }
    }
    return masks;
}

std::vector<U64> Masks::generateBlackPawnMoveMasks() 
{
    std::vector<U64> masks = std::vector<U64>(64,0);

    // pawns can't go on the first or last ranks
    for (int rank = 1; rank < 7; ++rank) {
        for (int file = 0; file < 8; ++file) {
            masks[rank*8 + file] = (1ull << ((rank-1)*8 + file));

            // if a black pawn is on the second last rank, it can go a second space downwards; not just once.
            if (rank == 6) {
                masks[rank*8 + file] |= (1ull << ((rank-2)*8 + file));
            }
        }
    }
    return masks;
}

std::vector<U64> Masks::generateWhitePawnAttackMasks() 
{
    std::vector<U64> masks = std::vector<U64>(64,0);
    for (int rank = 0; rank < 7; ++rank) {
        for (int file = 0; file < 8; ++file) {

            // a white pawn can attack the two pieces diagonal in front of it if it is not on the edge of the board.
            // if it is on the edge, it can only attack in one direction.
            if (file > 0 && file < 7) {
                masks[rank*8 + file] = (1ull << ((rank+1)*8 + file+1)) | (1ull << ((rank+1)*8 + file-1));
            } else if (file == 7) {
                masks[rank*8 + file] = (1ull << ((rank+1)*8 + file-1));
            } else {
                masks[rank*8 + file] = (1ull << ((rank+1)*8 + file+1));
            }
        }
    }
    return masks;
}

std::vector<U64> Masks::generateBlackPawnAttackMasks() 
{
    std::vector<U64> masks = std::vector<U64>(64,0);
    for (int rank = 1; rank < 8; ++rank) {
        for (int file = 0; file < 8; ++file) {

            // a black pawn can attack the two pieces diagonal below it if it is not on the edge of the board.
            // if it is on the edge, it can only attack in one direction.
            if (file > 0 && file < 7) {
                masks[rank*8 + file] = (1ull << ((rank-1)*8 + file+1)) | (1ull << ((rank-1)*8 + file-1));
            } else if (file == 7) {
                masks[rank*8 + file] = (1ull << ((rank-1)*8 + file-1));
            } else {
                masks[rank*8 + file] = (1ull << ((rank-1)*8 + file+1));
            }
        }
    }
    return masks;
}

std::vector<std::vector<U64>> Masks::generateAttackRay(const int fileDelta, const int rankDelta)
{
    std::vector<std::vector<U64>> masks(64);
    for (int rank = 0; rank < 8; ++rank) {
        for (int file = 0; file < 8; ++file) {

            // for each position on the board, keep casting a ray in the (fileDelta, rankDelta) direction.
            std::vector<U64> raysFromTile;
            int curFile = file + fileDelta;
            int curRank = rank + rankDelta;
            while (0 <= curFile && curFile < 8 && 0 <= curRank && curRank < 8) {

                // as the ray is being casted as far as it can go in the (fileDelta, rankDelta) direction,
                // keep adding on new bits/tiles in that direction.
                raysFromTile.push_back((1ull << (curRank*8 + curFile)));
                curFile += fileDelta;
                curRank += rankDelta;
            }
            masks[rank*8 + file] = raysFromTile;
        }
    }

    return masks;
}

const int Masks::knightPositionBonus[2][64] = {{

    // WHITE
    -50,   -25,  0,   -30, -30, 0,  -25,  -50,
    -20,   0,    0,   0,   25,   0,   0,  -20,
    -15,   0,    50,  15,  15,  50,  0,  -15,
    0,     0,    15,  20,  20,  15,  0,  0,
    0,     0,    15,  0,  0,  15,  0,  0,
    -10,   0,    0,  15,  15,  0,  0,  -10,
    -20,   0,    20,   0,   25,   20,   0,  -20,
    -50,   -25,  0,   -30, -30, 0,  -25,  -50,
}, {

    // BLACK
    -50,   -25,  0,   -30, -30, 0,  -25,  -50,
    -20,   0,    20,   0,   25,   20,   0,  -20,
    -15,   0,    0,  15,  15,  0,  0,  -15,
    0,     0,    15,  0,  0,  15,  0,  0,
    0,     0,    15,  20,  20,  15,  0,  0,
    -10,   0,    50,  15,  15,  50,  0,  -10,
    -20,   0,    0,   0,   25,   0,   0,  -20,
    -50,   -25,  0,   -30, -30, 0,  -25,  -50,
}};

const int Masks::bishopPositionBonus[2][64] ={{
    // WHITE
    0,   -70,  -50,  -30, -30, -50, -70,  0,
    -20, 25,   0,    10,  10,  0,   25,  -20,
    0,   0,    10,   25,  25,  10,  0,  0,
    0,   0,    30,   15,  15,  20,  0,  0,
    0,   20,    30,   15,  15,  20,  0,  0,
    0,   0,    10,   25,  25,  10,  0,  0,
    -20, 15,   0,    10,  10,  0,   15,  -20,
    -30,   -70,  -50,  -30, -30, -50, -70,  -30,
}, {

    // BLACK
    -30,   -70,  -50,  -30, -30, -50, -70,  -30,
    -20, 15,   0,    10,  10,  0,   15,  -20,
    0,   0,    10,   25,  25,  10,  0,  0,
    0,   20,    30,   15,  15,  20,  0,  0,
    0,   0,    30,   15,  15,  20,  0,  0,
    0,   0,    10,   25,  25,  10,  0,  0,
    -20, 25,   0,    10,  10,  0,   25,  -20,
    0,   -70,  -50,  -30, -30, -50, -70,  0,
}};

const int Masks::rookPositionBonus[2][64] ={{
    // WHITE
    0, -20, 0, 20, 10, 20, -20, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -10, -10, 0, 0, 0,
    0, 0, 0, -10, -10, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    20, 20, 20, 20, 20, 20, 20, 20,
    0, 0, 0, 0, 0, 0, 0, 0,

}, {

    // BLACK
    0, 0, 0, 0, 0, 0, 0, 0,
    20, 20, 20, 20, 20, 20, 20, 20,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -10, -10, 0, 0, 0,
    0, 0, 0, -10, -10, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -20, 0, 20, 10, 20, -20, 0,
}};

const int Masks::queenPositionBonus[2][64] ={{
    // WHITE
    -20, 0, 0, 0, 0, 0, 0, -20,
    -40, 0, 0, 0, 0, 0, 0, -40,
    -40, 0, 0, 0, 0, 0, 0, -40,
    0, 0, 0, -35, -35, 0, 0, 0,
    0, 0, 0, -35, -35, 0, 0, 0,
    0, 0, 0, 0, -10, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,

}, {

    // BLACK
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -10, 0, 0, 0,
    0, 0, 0, -35, -35, 0, 0, 0,
    0, 0, 0, -35, -35, 0, 0, 0,
    -40, 0, 0, 0, 0, 0, 0, -40,
    -40, 0, 0, 0, 0, 0, 0, -40,
    -20, 0, 0, 0, 0, 0, 0, -20,
}};

const int Masks::whiteKingEarlyGame[64] = {
    0, 0, 30, -10, 0, -10, 30, 20
    -30, -30, -30, -30, -30, -30, -30, -30,
    -300, -300, -300, -300, -300, -300, -300, -300,
    -400, -400, -400, -400, -400, -400, -400, -400,
    -400, -400, -400, -400, -400, -400, -400, -400,
    -400, -400, -400, -400, -400, -400, -400, -400,
    -400, -400, -400, -400, -400, -400, -400, -400,
    -400, -400, -400, -400, -400, -400, -400, -400,
};

const int Masks::blackKingEarlyGame[64] = {
    -400, -400, -400, -400, -400, -400, -400, -400,
    -400, -400, -400, -400, -400, -400, -400, -400,
    -400, -400, -400, -400, -400, -400, -400, -400,
    -400, -400, -400, -400, -400, -400, -400, -400,
    -400, -400, -400, -400, -400, -400, -400, -400,
    -300, -300, -300, -300, -300, -300, -300, -300,
    -30, -30, -30, -30, -30, -30, -30, -30,
    0, 0, 30, -10, 0, -10, 30, 20
};

const int Masks::centreManhattanDistance[64] = { 
    6, 5, 4, 3, 3, 4, 5, 6,
    5, 4, 3, 2, 2, 3, 4, 5,
    4, 3, 2, 1, 1, 2, 3, 4,
    3, 2, 1, 0, 0, 1, 2, 3,
    3, 2, 1, 0, 0, 1, 2, 3,
    4, 3, 2, 1, 1, 2, 3, 4,
    5, 4, 3, 2, 2, 3, 4, 5,
    6, 5, 4, 3, 3, 4, 5, 6
};

const int Masks::enemyKingPositionWithBlackBishop[64] = { 
    6, 5, 4, 3, -3, -4, -5, -6, // first rank
    5, 4, 3, 2, -2, -3, -4, -5,
    4, 3, 2, 1, -1, -2, -3, -4,
    3, 2, 1, 0, 0, -1, -2, -3,
    -3, -2, -1, 0, 0, 1, 2, 3,
    -4, -3, -2, -1, 1, 2, 3, 4,
    -5, -4, -3, -2, 2, 3, 4, 5,
    -6, -5, -4, -3, 3, 4, 5, 6 // last rank
};

const int Masks::enemyKingPositionWithWhiteBishop[64] = { 
    -6, -5, -4, -3, 3, 4, 5, 6, // first rank
    -5, -4, -3, -2, 2, 3, 4, 5,
    -4, -3, -2, -1, 1, 2, 3, 4,
    -3, -2, -1, 0, 0, 1, 2, 3,
    3, 2, 1, 0, 0, -1, -2, -3,
    4, 3, 2, 1, -1, -2, -3, -4,
    5, 4, 3, 2, -2, -3, -4, -5,
    6, 5, 4, 3, -3, -4, -5, -6 // last rank
};
