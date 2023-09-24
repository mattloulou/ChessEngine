#ifndef HASH_H
#define HASH_H

#include "BitBoard.h"
#include <unordered_map>

class Hash {
    // a hash table for the min player and the max player, since a collision would be very bad between the two
    std::unordered_map<U64, U64> minTable;
    std::unordered_map<U64, U64> maxTable;
    // our mod value for the hash (large prime)
    const U64 mod = static_cast<int>(1e9 + 7);
    // obtain the hash for a base, exponent pair using our mod
    U64 hashPower(U64 base, U64 exponent) const;
public:
    // given a board and a depth, get our hash key using complete information from the board
    U64 getHashKey(const BitBoard& board, const int depth) const;
    // set a key value pair in our table for min/max turn
    void setKeyValue(const U64 key, const bool maxTurn, const U64 value);
    // get value for given key and min/max turn
    U64 getValue(const U64 key, const bool maxTurn);
    // check if key exists for min/max turn
    bool keyExists(const U64 key, const bool maxTurn) const;

    // clears the hash tables
    void clearHashTables();
};

#endif
