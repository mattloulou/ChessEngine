#include "Hash.h"

U64 Hash::hashPower(U64 base, U64 exponent) const
{
    // calculate the power value, taking mod each time to avoid overflow
    U64 ans = 1;
    base %= mod;
    while (exponent > 0) {
        ans *= base;
        ans %= mod;
        exponent--;
    }
    return ans;
}

U64 Hash::getHashKey(const BitBoard& board, const int depth) const
{
    // assign exponent 13-2 to the bitboard state, and sum these
    U64 exponent = 13;
    U64 hash = 0;
    for (int i = 0; i < 12; i++) {
        hash += hashPower(board.pieces[i], exponent);
        hash %= mod;
        exponent--;
    }
    // assign exponent 1 to the temporal info
    hash += hashPower(board.temporalInfo, exponent);
    hash %= mod;
    // assign exponent 0 to the depth
    hash += depth;
    hash %= mod;
    return hash;
}

void Hash::setKeyValue(const U64 key, const bool maxTurn, const U64 value)
{
    if (maxTurn) {
        maxTable[key] = value;
    } else {
        minTable[key] = value;
    }
}

U64 Hash::getValue(const U64 key, const bool maxTurn)
{
    if (maxTurn) {
        return maxTable[key];
    } else {
        return minTable[key];
    }
}

bool Hash::keyExists(const U64 key, const bool maxTurn) const
{
    if (maxTurn) {
        return maxTable.count(key);
    } else {
        return minTable.count(key);
    }
}

void Hash::clearHashTables()
{
    maxTable.clear();
    minTable.clear();
}