#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <memory>
#include <numeric>
#include "ChessSquares.h"
#include "BitBoard.h"
#include "Masks.h"
const int KING_VALUE = 1000000;
const int QUEEN_VALUE = 900;
const int ROOK_VALUE = 500;
const int BISHOP_VALUE = 300;
const int KNIGHT_VALUE = 300;
const int PAWN_VALUE = 100;

inline bool BitBoard::isWhitePawnEnpassantable(const unsigned file) const
{
    return temporalInfo & (1u << file);
}

inline bool BitBoard::isBlackPawnEnpassantable(const unsigned file) const
{
    return temporalInfo & (0x100u << file);
}

inline void BitBoard::setWhitePawnEnpassantable(const unsigned file)
{
    temporalInfo |= (1u << file);
}

inline void BitBoard::setBlackPawnEnpassantable(const unsigned file)
{
    temporalInfo |= (0x100u << file);
}

inline void BitBoard::resetEnpassantFlags()
{
    temporalInfo &= ~(0xFFFFu);
}

inline bool BitBoard::isWhiteLeftCastleable() const
{
    return (temporalInfo & 0x110000u) == 0x110000u;
}

inline bool BitBoard::isWhiteRightCastleable() const
{
    return (temporalInfo & 0x120000u) == 0x120000u;
}

inline bool BitBoard::isBlackLeftCastleable() const
{
    return (temporalInfo & 0x280000) == 0x280000;
}

inline bool BitBoard::isBlackRightCastleable() const
{
    return (temporalInfo & 0x240000) == 0x240000;
}

inline void BitBoard::setWhiteKingMoved()
{
    temporalInfo &= ~0x100000u;
}

inline void BitBoard::setBlackKingMoved()
{
    temporalInfo &= ~0x200000u;
}

inline void BitBoard::setWhiteLeftRookMoved()
{
    temporalInfo &= ~0x10000u;
}

inline void BitBoard::setWhiteRightRookMoved()
{
    temporalInfo &= ~0x20000u;
}

inline void BitBoard::setBlackRightRookMoved()
{
    temporalInfo &= ~0x40000u;
}

inline void BitBoard::setBlackLeftRookMoved()
{
    temporalInfo &= ~0x80000u;
}

inline int BitBoard::getCurrentPlayerOffset() const
{
    return 6*!!(temporalInfo & 0x400000u);
}

inline int BitBoard::getOtherPlayerOffset() const
{
    return 6-getCurrentPlayerOffset();
}

inline void BitBoard::swapCurrentPlayer()
{
    temporalInfo ^= 0x400000u;
}

std::vector<Piece> BitBoard::getPieceList() const
{
    // set up the board with tiles
    std::vector<Piece> boardOutput(64, Piece::DarkNone);

    // every second tile should be light
    U64 lightSquares = Masks::checkerboardPattern;
    while (lightSquares > 0) {
        const int index = std::countr_zero(lightSquares);
        boardOutput[index] = Piece::LightNone;
        lightSquares &= ~(1ull << index);
    }
    
    // put the pieces on the board
    for (int i = 0; i < 12; ++i) {
        U64 pieceMask = this->pieces[i];
        while (pieceMask > 0) {
            boardOutput[std::countr_zero(pieceMask)] = Piece(i);
            pieceMask &= ~(1ull << std::countr_zero(pieceMask));
        }
    }
    return boardOutput;
}

void BitBoard::debugPrint() const
{
    unordered_map<Piece,string> pieceMap = {
            {Piece::K," ♔"}, {Piece::Q," ♕"}, {Piece::B," ♗"}, {Piece::N," ♘"}, 
            {Piece::R," ♖"}, {Piece::P," ♙"}, {Piece::k," ♚"}, {Piece::q," ♛"}, 
            {Piece::b," ♝"}, {Piece::n," ♞"}, {Piece::r," ♜"}, {Piece::p," ♟︎"},
            {Piece::DarkNone,"⬛"}, {Piece::LightNone,"⬜"}};

    // set up the board with tiles
    std::vector<string> boardOutput;
    auto pieceList = getPieceList();
    for (auto p : pieceList) {
        boardOutput.push_back(pieceMap[p]);
    }

    // print the board
    for (int rank = 7; rank >= 0; --rank) {
        std::cout << (rank+1) << " ";
        for (int file = 0; file < 8; ++file) {
            std::cout << boardOutput[rank*8 + file];
        }
        std::cout << std::endl;
    }
    std::cout << "   A B C D E F G H" << endl;

    // check if any pieces share a position
    for (int i = 0; i < 11; ++i) {
        for (int j = i + 1; j < 12; ++j) {
            if ((pieces[i] & pieces[j]) != 0) {
                cout << "WARNING: SOME PIECES IN PIECE MASK " << i << " AND " << j << " OVERLAP" << std::endl;
            }
        }
    }

    // output temporalInfo
    std::cout << "white pawns can be enpassanted?:" << std::endl << "  ";
    for (int i = 0; i < 8; ++i) {
        std::cout << (isWhitePawnEnpassantable(i) ? "🟩": "🟥");
    }
    std::cout << std::endl;

    std::cout << "black pawns can be enpassanted?:" << std::endl << "  ";
    for (int i = 0; i < 8; ++i) {
        std::cout << (isBlackPawnEnpassantable(i) ? "🟩": "🟥");
    }
    std::cout << std::endl;

    std::cout << "castling rights (associated king and rook haven't moved)?:" << std::endl;
    std::cout << "black" << (isBlackLeftCastleable() ? "🟩": "🟥") << (isBlackRightCastleable() ? "🟩": "🟥") << std::endl;
    std::cout << "white" << (isWhiteLeftCastleable() ? "🟩": "🟥") << (isWhiteRightCastleable() ? "🟩": "🟥") << std::endl;

    const BoardState state = getState();
    std::cout << "wins: white | draw | black" << std::endl;
    std::cout << "        " << (state.whiteHasWin ? "🟩": "🟥") << "     " << (state.hasDraw ? "🟩": "🟥") << "     " << (state.blackHasWin ? "🟩": "🟥") << std::endl;
    std::cout << "in check: white | black" << std::endl;
    std::cout << "           " << (state.whiteInCheck ? "🟩": "🟥") << "       " << (state.blackInCheck ? "🟩": "🟥") << std::endl;
}

inline U64 BitBoard::getPlayerCombinedPieceMask(const Player player) const
{
    U64 mask = 0ull;
    for (int i = Piece::K; i < Piece::k; ++i) {
        mask |= pieces[player + i];
    }
    return mask;
}

void BitBoard::useRayMask(std::vector<Move>& nonCaptureMoves, std::vector<Move>& captureMoves, const Position position, const U64 friendMask, const U64 enemyMask, const std::vector<U64>& masks)
{
    const U64 combinedMask = friendMask | enemyMask;
    
    // cast ray
    for (auto mask : masks) {
        
        // this means there was a collision
        if (combinedMask & mask) {

            // this means it collided with an enemy piece, so it is a candidate for a legal attacking move
            if (mask & enemyMask) {
                captureMoves.push_back({position, std::countr_zero(mask)});
            }
            return;
        
        // this means there was no collision, so it is a candidate to be a valid `moving` move.
        } else {
            nonCaptureMoves.push_back({position, std::countr_zero(mask)});
        }
    }
}

inline void BitBoard::checkRayForCheck(std::vector<Position>& enemyChecks, const U64 nonhostileMask, const U64 attackingEnemyMask, const std::vector<U64>& masks)
{
    const U64 combinedMask = nonhostileMask | attackingEnemyMask;

    // cast ray
    for (auto mask : masks) {
        
        // this means there was a collision
        if (mask & combinedMask) {

            // this means it collided with an enemy piece, so it is a candidate for a legal attacking move
            if (mask & attackingEnemyMask) {

                enemyChecks.push_back(std::countr_zero(mask));
            } 
            return;
        } 
    }
}


inline void BitBoard::candidateBishopMoves(std::vector<Move>& nonCaptureMoves, std::vector<Move>& captureMoves, const Position position, const U64 friendMask, const U64 enemyMask, shared_ptr<Masks> masks)
{
    useRayMask(nonCaptureMoves, captureMoves, position, friendMask, enemyMask, masks->attackRayDiagonalUpRight[position.position]);
    useRayMask(nonCaptureMoves, captureMoves, position, friendMask, enemyMask, masks->attackRayDiagonalUpLeft[position.position]);
    useRayMask(nonCaptureMoves, captureMoves, position, friendMask, enemyMask, masks->attackRayDiagonalDownRight[position.position]);
    useRayMask(nonCaptureMoves, captureMoves, position, friendMask, enemyMask, masks->attackRayDiagonalDownLeft[position.position]);
}

inline void BitBoard::candidateRookMoves(std::vector<Move>& nonCaptureMoves, std::vector<Move>& captureMoves, const Position position, const U64 friendMask, const U64 enemyMask, shared_ptr<Masks> masks)
{
    useRayMask(nonCaptureMoves, captureMoves, position, friendMask, enemyMask, masks->attackRayRight[position.position]);
    useRayMask(nonCaptureMoves, captureMoves, position, friendMask, enemyMask, masks->attackRayUp[position.position]);
    useRayMask(nonCaptureMoves, captureMoves, position, friendMask, enemyMask, masks->attackRayLeft[position.position]);
    useRayMask(nonCaptureMoves, captureMoves, position, friendMask, enemyMask, masks->attackRayDown[position.position]);
}

inline void BitBoard::candidateQueenMoves(std::vector<Move>& nonCaptureMoves, std::vector<Move>& captureMoves, const Position position, const U64 friendMask, const U64 enemyMask, shared_ptr<Masks> masks)
{
    candidateBishopMoves(nonCaptureMoves, captureMoves, position, friendMask, enemyMask, masks);
    candidateRookMoves(nonCaptureMoves, captureMoves, position, friendMask, enemyMask, masks);
}

inline void BitBoard::candidatePawnMoves(std::vector<Move>& nonCaptureMoves, std::vector<Move>& captureMoves, const Position position, const U64 friendMask, const U64 enemyMask, const U64 attackMask, const U64 moveMask) const
{
    // First, we find moves that don't take pieces
    U64 possibleMoves = moveMask & ~(friendMask | enemyMask);
    
    // we can only move if it is not the case that we can do a double move, where the piece adjacent to this pawn is blocked, 
    // but the tile beyond it isn't
    if (!(std::popcount(moveMask) == 2 && std::popcount(possibleMoves) == 1 && abs(std::countr_zero(possibleMoves)-position.position) > 8)) {
        while (possibleMoves > 0) {
            nonCaptureMoves.push_back({position, std::countr_zero(possibleMoves)});
            possibleMoves &= ~(1ull << std::countr_zero(possibleMoves));
        }
    }

    // check attacking moves
    possibleMoves = attackMask & enemyMask; // can only attack enemies
    while (possibleMoves > 0) {
        captureMoves.push_back({position, std::countr_zero(possibleMoves)});
        possibleMoves &= ~(1ull << std::countr_zero(possibleMoves));
    }
}

inline void BitBoard::candidateWhitePawnMoves(std::vector<Move>& nonCaptureMoves, std::vector<Move>& captureMoves, const Position position, const U64 friendMask, const U64 enemyMask, shared_ptr<Masks> masks) const
{
    // Get a mask that puts any black pawns that just double-moved onto the spot
    // where they would have been if they had single moved, so that this pawn can
    // en passant them
    const U64 enPassantMask = ((temporalInfo & 0xFF00ull) << 32);
    candidatePawnMoves(nonCaptureMoves, captureMoves, position, friendMask, enemyMask | enPassantMask, masks->whitePawnAttackMasks[position.position], masks->whitePawnMoveMasks[position.position]);
}

inline void BitBoard::candidateBlackPawnMoves(std::vector<Move>& nonCaptureMoves, std::vector<Move>& captureMoves, const Position position, const U64 friendMask, const U64 enemyMask, shared_ptr<Masks> masks) const
{
    // Get a mask that puts any white pawns that just double-moved onto the spot
    // where they would have been if they had single moved, so that this pawn can
    // en passant them
    const U64 enPassantMask = ((temporalInfo & 0xFFu) << 16);
    candidatePawnMoves(nonCaptureMoves, captureMoves, position, friendMask, enemyMask | enPassantMask, masks->blackPawnAttackMasks[position.position], masks->blackPawnMoveMasks[position.position]);
}

inline void BitBoard::candidateKnightMoves(std::vector<Move>& nonCaptureMoves, std::vector<Move>& captureMoves, const Position position, const U64 friendMask, const U64 enemyMask, shared_ptr<Masks> masks)
{
    U64 possibleMoves = masks->knightAttackMasks[position.position] & ~friendMask; // can't move to where a friendly piece is
    while (possibleMoves > 0) {
        const int newPosition = std::countr_zero(possibleMoves);
        if (enemyMask & (1ull << newPosition)) {
            captureMoves.push_back({position, newPosition});
        } else {
            nonCaptureMoves.push_back({position, newPosition});
        }
        possibleMoves &= ~(1ull << newPosition);
    }
}

inline void BitBoard::candidateKingMoves(std::vector<Move>& nonCaptureMoves, std::vector<Move>& captureMoves, const Position position, const U64 friendMask, const U64 enemyMask, shared_ptr<Masks> masks)
{
    U64 possibleMoves = masks->kingAttackMasks[position.position] & ~friendMask; // can't move to where a friendly piece is
    while (possibleMoves > 0) {
        const int newPosition = std::countr_zero(possibleMoves);
        if (enemyMask & (1ull << newPosition)) {
            captureMoves.push_back({position, newPosition});
        } else {
            nonCaptureMoves.push_back({position, newPosition});
        }
        possibleMoves &= ~(1ull << newPosition);
    }
}

inline void BitBoard::checkForKnightChecks(std::vector<Position>& enemyChecks, const Position position, const U64 enemyMask, shared_ptr<Masks> masks) const
{
    // `& enemyMask` is so that if we are using this method in `isKingInCheck`, for checking if a move took the king out of check
    // by killing a knight that was putting our king in check, we don't count the knight that was killed
    U64 possibleKnightAttacks = masks->knightAttackMasks[position.position] & pieces[getOtherPlayerOffset() + Piece::N] & enemyMask;
    while (possibleKnightAttacks > 0) {
        enemyChecks.push_back(std::countr_zero(possibleKnightAttacks));
        possibleKnightAttacks &= ~(1ull << std::countr_zero(possibleKnightAttacks));
    }
}

inline void BitBoard::checkForHorizontalVerticalChecks(std::vector<Position>& enemyChecks, const Position position, const U64 friendMask, const U64 enemyMask, shared_ptr<Masks> masks) const
{
    const int otherPlayerOffset = getOtherPlayerOffset();

    // only consider rooks and queens (only they can attack horizontally and vertically).
    // `& enemyMask` is so that if we are using this method in `isKingInCheck`, for checking if a move takes the king out of check
    // by killing a rook/queen that was putting our king in check, we don't count the piece that was killed
   
    const U64 hostileEnemyMask = (pieces[otherPlayerOffset + Piece::Q] | pieces[otherPlayerOffset + Piece::R]) & enemyMask;
    const U64 nonhostileEnemyMask = enemyMask & ~(hostileEnemyMask);

    checkRayForCheck(enemyChecks, (friendMask | nonhostileEnemyMask), hostileEnemyMask, masks->attackRayRight[position.position]);
    checkRayForCheck(enemyChecks, (friendMask | nonhostileEnemyMask), hostileEnemyMask, masks->attackRayUp[position.position]);
    checkRayForCheck(enemyChecks, (friendMask | nonhostileEnemyMask), hostileEnemyMask, masks->attackRayLeft[position.position]);
    checkRayForCheck(enemyChecks, (friendMask | nonhostileEnemyMask), hostileEnemyMask, masks->attackRayDown[position.position]);
}

inline void BitBoard::checkForDiagonalChecks(std::vector<Position>& enemyChecks, const Position position, const U64 friendMask, const U64 enemyMask, shared_ptr<Masks> masks) const
{
    const int otherPlayerOffset = getOtherPlayerOffset();

    // get the enemy pawns that could potentially attack the piece at `position`. These are equivalently the pawns that `position` could attack, if it were itself a pawn.
    // ternary operator case explanation: if the other player offset is 0, then we are black. Otherwise, we are white.
    const U64 potentialPawnAttacks = pieces[otherPlayerOffset + Piece::P] & ((otherPlayerOffset == Player::White) ? masks->blackPawnAttackMasks[position.position] : masks->whitePawnAttackMasks[position.position]);
    
    // only consider bishops and queens (only they can attack horizontally and vertically)
    // `& enemyMask` is so that if we are using this method in `isKingInCheck`, for checking if a move takes the king out of check
    // by killing a rook/queen/pawn that was putting our king in check, we don't count the piece that was killed
    const U64 hostileEnemyMask = (pieces[otherPlayerOffset+Piece::Q] | pieces[otherPlayerOffset+Piece::B] | potentialPawnAttacks) & enemyMask;
    const U64 nonhostileEnemyMask = enemyMask & ~(hostileEnemyMask);

    checkRayForCheck(enemyChecks, (friendMask | nonhostileEnemyMask), hostileEnemyMask, masks->attackRayDiagonalUpRight[position.position]);
    checkRayForCheck(enemyChecks, (friendMask | nonhostileEnemyMask), hostileEnemyMask, masks->attackRayDiagonalUpLeft[position.position]);
    checkRayForCheck(enemyChecks, (friendMask | nonhostileEnemyMask), hostileEnemyMask, masks->attackRayDiagonalDownLeft[position.position]);
    checkRayForCheck(enemyChecks, (friendMask | nonhostileEnemyMask), hostileEnemyMask, masks->attackRayDiagonalDownRight[position.position]);
}

inline void BitBoard::checkForEnemyKingChecks(std::vector<Position>& enemyChecks, const Position yourKingPosition, shared_ptr<Masks> masks) const
{
    const int otherPlayerOffset = getOtherPlayerOffset();

    const int enemyKingPosition = std::countr_zero(pieces[otherPlayerOffset + Piece::K]);
    if (masks->kingAttackMasks[enemyKingPosition] & (1ull << yourKingPosition.position)) {
        enemyChecks.push_back(enemyKingPosition);
    }
}

// precondition; requires both kings to be on the board
// `kinglessFriendMask` should not have your king in it. The king's position should only taken into account by `kingPosition`
inline bool BitBoard::isKingInCheck(const Position kingPosition, const U64 kinglessFriendMask, const U64 enemyMask, shared_ptr<Masks> masks) const
{
    std::vector<Position> enemyCheckingPieces;
    
    // check enemy queens and rooks
    checkForHorizontalVerticalChecks(enemyCheckingPieces, kingPosition, kinglessFriendMask, enemyMask, masks);
    if (enemyCheckingPieces.size()) return true;
    
    // check enemy queens, bishops, and pawns
    checkForDiagonalChecks(enemyCheckingPieces, kingPosition, kinglessFriendMask, enemyMask, masks);
    if (enemyCheckingPieces.size()) return true;
    
    // check enemy knights
    checkForKnightChecks(enemyCheckingPieces, kingPosition, enemyMask, masks);
    if (enemyCheckingPieces.size()) return true;

    // check enemy king
    checkForEnemyKingChecks(enemyCheckingPieces, kingPosition, masks);
    return enemyCheckingPieces.size();
}

inline int BitBoard::findPieceOffset(const Position position, const int playerOffset) const
{
    for (int i = 0; i < 6; ++i) {
        if (pieces[playerOffset + i] & (1ull << position.position)) {
            return i;
        }
    }
    return -1;
}

inline void BitBoard::movePiece(U64& tile, const Move& m)
{
    tile &= ~(1ull << m.start.position);
    tile |= (1ull << m.end.position);
}

inline void BitBoard::applyKingMove(const Move& move)
{
    const int currentPlayerOffset = getCurrentPlayerOffset();

    resetEnpassantFlags();

    pieces[currentPlayerOffset + Piece::K] = (1ull << move.end.position);
    const int enemyPieceOffset = findPieceOffset(move.end, getOtherPlayerOffset());
    if (enemyPieceOffset != -1) {
        pieces[getOtherPlayerOffset() + enemyPieceOffset] &= ~(1ull << move.end.position);
    }
    if (currentPlayerOffset == 0) {
        setWhiteKingMoved();
    } else {
        setBlackKingMoved();
    }
}

inline void BitBoard::applyQueenMove(const Move& move)
{
    const int currentPlayerOffset = getCurrentPlayerOffset();

    resetEnpassantFlags();

    // put the queen at her new place
    movePiece(pieces[currentPlayerOffset + Piece::Q], move);

    // check if this was a capturing move. If it was, then remove the opponent's piece
    const int enemyPieceOffset = findPieceOffset(move.end, getOtherPlayerOffset());
    if (enemyPieceOffset != -1) {
        pieces[getOtherPlayerOffset() + enemyPieceOffset] &= ~(1ull << move.end.position);
    }
}

inline void BitBoard::applyBishopMove(const Move& move)
{
    const int currentPlayerOffset = getCurrentPlayerOffset();

    resetEnpassantFlags();

    // put the bishop at her new place
    movePiece(pieces[currentPlayerOffset + Piece::B], move);


    // check if this was a capturing move. If it was, then remove the opponent's piece
    const int enemyPieceOffset = findPieceOffset(move.end, getOtherPlayerOffset());
    if (enemyPieceOffset != -1) {
        pieces[getOtherPlayerOffset()+enemyPieceOffset] &= ~(1ull << move.end.position);
    }
}

void BitBoard::applyKnightMove(const Move& move)
{
    const int currentPlayerOffset = getCurrentPlayerOffset();

    resetEnpassantFlags();

    // put the knight at his new place
    movePiece(pieces[currentPlayerOffset + Piece::N], move);

    // check if this was a capturing move. If it was, then remove the opponent's piece
    const int enemyPieceOffset = findPieceOffset(move.end, getOtherPlayerOffset());
    if (enemyPieceOffset != -1) {
        pieces[getOtherPlayerOffset() + enemyPieceOffset] &= ~(1ull << move.end.position);
    }
}

inline void BitBoard::applyRookMove(const Move& move)
{
    const int currentPlayerOffset = getCurrentPlayerOffset();

    resetEnpassantFlags();

    // put the rook at its new place
    movePiece(pieces[currentPlayerOffset + Piece::R], move);


    // check if this was a capturing move. If it was, then remove the opponent's piece
    const int enemyPieceOffset = findPieceOffset(move.end, getOtherPlayerOffset());
    if (enemyPieceOffset != -1) {
        pieces[getOtherPlayerOffset() + enemyPieceOffset] &= ~(1ull << move.end.position);
    } 

    // update flags for castleability
    if (currentPlayerOffset == Player::White) {
        if (move.start.position == A1) {
            setWhiteLeftRookMoved();
        } else if (move.start.position == H1) {
            setWhiteRightRookMoved();
        }
    } else {
        if (move.start.position == A8) {
            setBlackLeftRookMoved();
        } else if (move.start.position == H8) {
            setBlackRightRookMoved();
        }
    }
}

inline void BitBoard::applyPawnMove(const Move& move)
{
    const int currentPlayerOffset = getCurrentPlayerOffset();

    resetEnpassantFlags();

    // put the pawn at its new place
    movePiece(pieces[currentPlayerOffset + Piece::P], move);

    // check if this was a normal capturing move. If it was, then remove the opponent's piece
    const int enemyPlayerOffset = getOtherPlayerOffset();
    const int enemyPieceOffset = findPieceOffset(move.end, enemyPlayerOffset);
    if (enemyPieceOffset != -1) {
        pieces[enemyPlayerOffset + enemyPieceOffset] &= ~(1ull << move.end.position);
    
    // check if this was an en passant attack. That will be when the attacking pawn moved off of its file,
    // yet there was no piece there.
    } else if (move.end.getFile() != move.start.getFile()) {
        const int enemyPosition = move.start.getRank()*8 + move.end.getFile();
        pieces[enemyPlayerOffset + Piece::P] &= ~(1ull << enemyPosition);
    }

    // apply the enpassant flag
    if (abs(move.start.position - move.end.position) == 16) {
        if (currentPlayerOffset == 0) {
            setWhitePawnEnpassantable(move.end.getFile());
        } else {
            setBlackPawnEnpassantable(move.end.getFile());
        }
    }
}

// precondition: king and rook are on the same rank, and both exist at the positions specified
// precondition: king is not in check at their initial location
inline bool BitBoard::checkCastleable(const int kingPosition, const int rookPosition, const U64 friendMask, const U64 enemyMask, std::shared_ptr<Masks> masks) const
{
    // make sure the rook exists at the spot it says it does
    const int currentPlayerOffset = getCurrentPlayerOffset();
    if ((pieces[currentPlayerOffset + Piece::R] & (1ull << rookPosition)) == 0) {
        return false;
    }

    const int rookDirection = (rookPosition - kingPosition)/abs(rookPosition - kingPosition);

    // make sure there are no pieces between the king and the rook.
    U64 betweenSection = 0ull;

    // this implementation only works for horizontal castling. If we had a weird random board setup with vertical rook/king, then
    // it would not work.
    for (int i = kingPosition + rookDirection; i != rookPosition; i += rookDirection) {
        betweenSection |= (1ull << i);
    }
    if ((betweenSection & (enemyMask | friendMask)) != 0) {
        return false;
    }

    // check if tile beside king is in check
    {
        const U64 kinglessFriendMask = friendMask & ~(pieces[currentPlayerOffset]);
        const int newKingPosition = kingPosition + rookDirection;
        // enemy mask must not have changed, since there should be no pieces between the kings
        if (isKingInCheck(newKingPosition, kinglessFriendMask, enemyMask, masks)) {
            return false;
        }
    }

    // check the king's destination tile is in check
    {
        const U64 kinglessFriendMask = friendMask & ~(pieces[currentPlayerOffset]);
        const int newKingPosition = kingPosition + (2 * rookDirection);
        // enemy mask must not have changed, since there should be no pieces between the kings
        if (isKingInCheck(newKingPosition, kinglessFriendMask, enemyMask, masks)) {
            return false;
        }
    }

    return true;
}

inline void BitBoard::applyAllPawnMoves(std::vector<std::shared_ptr<BitBoard>>& result, std::vector<Move>& moves, const int kingPosition, const U64 friendMask, const U64 enemyMask, std::shared_ptr<Masks> masks) const
{
    for (auto& m : moves) {
        const U64 newFriendMask = (friendMask & ~(1ull << m.start.position)) | (1ull << m.end.position);
        const U64 newEnemyMask = enemyMask & ~(1ull << m.end.position);
        if (!isKingInCheck(kingPosition, newFriendMask, newEnemyMask, masks)) {

            // promotion pawn move.
            if (m.end.getRank() == 0 || m.end.getRank() == 7) {
                std::vector<Piece> promotionPieces = {Piece(Piece::Q), Piece(Piece::N), Piece(Piece::R), Piece(Piece::B)};
                
                for (auto piece : promotionPieces) {
                    auto newBoard = result.emplace_back(make_unique<BitBoard>(*this));
                    newBoard->applyPawnMove(m);
                    newBoard->pieces[getCurrentPlayerOffset() + Piece::P] &= ~(1ull << m.end.position);
                    newBoard->pieces[getCurrentPlayerOffset() + piece] |= (1ull << m.end.position);
                    newBoard->swapCurrentPlayer();
                }
                
            // non-promotion pawn move
            } else {
                auto newBoard = result.emplace_back(make_unique<BitBoard>(*this));
                newBoard->applyPawnMove(m);
                newBoard->swapCurrentPlayer();
            }
        }
    }
}

inline void BitBoard::applyAllBishopMoves(std::vector<std::shared_ptr<BitBoard>>& result, std::vector<Move>& moves, const int kingPosition, const U64 friendMask, const U64 enemyMask, std::shared_ptr<Masks> masks) const
{
    for (auto& m : moves) {
        const U64 newFriendMask = (friendMask & ~(1ull << m.start.position)) | (1ull << m.end.position);
        const U64 newEnemyMask = enemyMask & ~(1ull << m.end.position);
        if (!isKingInCheck(kingPosition, newFriendMask, newEnemyMask, masks)) {
            auto newBoard = result.emplace_back(make_unique<BitBoard>(*this));
            newBoard->applyBishopMove(m);
            newBoard->swapCurrentPlayer();
        }
    }
}

inline void BitBoard::applyAllKnightMoves(std::vector<std::shared_ptr<BitBoard>>& result, std::vector<Move>& moves, const int kingPosition, const U64 friendMask, const U64 enemyMask, std::shared_ptr<Masks> masks) const
{
    for (auto& m : moves) {
        const U64 newFriendMask = (friendMask & ~(1ull << m.start.position)) | (1ull << m.end.position);
        const U64 newEnemyMask = enemyMask & ~(1ull << m.end.position);
        if (!isKingInCheck(kingPosition, newFriendMask, newEnemyMask, masks)) {
            auto newBoard = result.emplace_back(make_unique<BitBoard>(*this));
            newBoard->applyKnightMove(m);
            newBoard->swapCurrentPlayer();
        }
    }
}

inline void BitBoard::applyAllRookMoves(std::vector<std::shared_ptr<BitBoard>>& result, std::vector<Move>& moves, const int kingPosition, const U64 friendMask, const U64 enemyMask, std::shared_ptr<Masks> masks) const
{
    for (auto& m : moves) {
        const U64 newFriendMask = (friendMask & ~(1ull << m.start.position)) | (1ull << m.end.position);
        const U64 newEnemyMask = enemyMask & ~(1ull << m.end.position);
        if (!isKingInCheck(kingPosition, newFriendMask, newEnemyMask, masks)) {
            auto newBoard = result.emplace_back(make_unique<BitBoard>(*this));
            newBoard->applyRookMove(m);
            newBoard->swapCurrentPlayer();
        }
    }
}

inline void BitBoard::applyAllQueenMoves(std::vector<std::shared_ptr<BitBoard>>& result, std::vector<Move>& moves, const int kingPosition, const U64 friendMask, const U64 enemyMask, std::shared_ptr<Masks> masks) const
{
    for (auto& m : moves) {
        const U64 newFriendMask = (friendMask & ~(1ull << m.start.position)) | (1ull << m.end.position);
        const U64 newEnemyMask = enemyMask & ~(1ull << m.end.position);
        if (!isKingInCheck(kingPosition, newFriendMask, newEnemyMask, masks)) {
            auto newBoard = result.emplace_back(make_unique<BitBoard>(*this));
            newBoard->applyQueenMove(m);
            newBoard->swapCurrentPlayer();
        }
    }
}

inline void BitBoard::applyAllKingMoves(std::vector<std::shared_ptr<BitBoard>>& result, std::vector<Move>& moves, const int kingPosition, const U64 friendMask, const U64 enemyMask, std::shared_ptr<Masks> masks) const
{
    for (auto& m : moves) {
        const U64 kinglessFriendMask = (friendMask & ~(1ull << m.start.position));
        const U64 newEnemyMask = enemyMask & ~(1ull << m.end.position);
        // for this invokation of isKingInCheck(), unlike the one for other pieces, as the king has moved, I must use the move
        // ending position, instead of the local variable `kingPosition`
        if (!isKingInCheck(m.end.position, kinglessFriendMask, newEnemyMask, masks)) { 
            auto newBoard = result.emplace_back(make_unique<BitBoard>(*this));
            newBoard->applyKingMove(m);
            newBoard->swapCurrentPlayer();
        }
    }
}
    
std::vector<std::shared_ptr<BitBoard>> BitBoard::generateValidNextPositions() const
{
    const int currentPlayerOffset = getCurrentPlayerOffset();
    const auto masks = Masks::getMasks();

    const Position kingPosition = std::countr_zero(pieces[currentPlayerOffset]);
    const U64 enemyMask = getPlayerCombinedPieceMask(Player(getOtherPlayerOffset()));
    const U64 friendMask = getPlayerCombinedPieceMask(Player(getCurrentPlayerOffset()));



    // check for checking pieces
    std::vector<Position> enemyCheckingPieces;
    checkForKnightChecks(enemyCheckingPieces, kingPosition, enemyMask, masks);
    checkForHorizontalVerticalChecks(enemyCheckingPieces, kingPosition, friendMask, enemyMask, masks);
    checkForDiagonalChecks(enemyCheckingPieces, kingPosition, friendMask, enemyMask, masks);
    // don't need to check for enemy king checks because that would be impossible

    std::vector<Move> kingMoves, queenMoves, bishopMoves, knightMoves, rookMoves, pawnMoves;
    std::vector<Move> kingCaptureMoves, queenCaptureMoves, bishopCaptureMoves, knightCaptureMoves, rookCaptureMoves, pawnCaptureMoves;

    // STEP 1: in any kind of position, we could try to move our king.
    candidateKingMoves(kingMoves, kingCaptureMoves, kingPosition, friendMask, enemyMask, masks);


    // STEP 2: if we are not in double check, we may do other things (if our king is in double check, we must move it, and can't do anything else)
    if (enemyCheckingPieces.size() < 2) {

        // find all moves candidate moves (a friendly pieces moves with or without capturing, without going through a friendly piece where applicable)

        U64 ourQueens = pieces[currentPlayerOffset + Piece::Q];      
        U64 ourBishops = pieces[currentPlayerOffset + Piece::B];
        U64 ourKnights = pieces[currentPlayerOffset + Piece::N];
        U64 ourRook = pieces[currentPlayerOffset + Piece::R];
        U64 ourPawns = pieces[currentPlayerOffset + Piece::P];

        // these check for all valid moves, except for the fact that it ignores the illegality of ending in check.
        // I call these, `candidate moves`.
        while (ourQueens > 0) {
            const int piecePosition = std::countr_zero(ourQueens);
            candidateQueenMoves(queenMoves, queenCaptureMoves, piecePosition, friendMask, enemyMask, masks);
            ourQueens &= ~(1ull << piecePosition);
        }

        while (ourBishops > 0) {
            const int piecePosition = std::countr_zero(ourBishops);
            candidateBishopMoves(bishopMoves, bishopCaptureMoves, piecePosition, friendMask, enemyMask, masks);
            ourBishops &= ~(1ull << piecePosition);
        }

        while (ourKnights > 0) {
            const int piecePosition = std::countr_zero(ourKnights);
            candidateKnightMoves(knightMoves, knightCaptureMoves, piecePosition, friendMask, enemyMask, masks);
            ourKnights &= ~(1ull << piecePosition);
        }            

        while (ourRook > 0) {
            const int piecePosition = std::countr_zero(ourRook);
            candidateRookMoves(rookMoves, rookCaptureMoves, piecePosition, friendMask, enemyMask, masks);
            ourRook &= ~(1ull << piecePosition);
        }
        
        while (ourPawns > 0) {
            const int piecePosition = std::countr_zero(ourPawns);

            // finding pawn moves is different based on their logic
            if (currentPlayerOffset == Player::White) {
                candidateWhitePawnMoves(pawnMoves, pawnCaptureMoves, piecePosition, friendMask, enemyMask, masks);
            } else {
                candidateBlackPawnMoves(pawnMoves, pawnCaptureMoves, piecePosition, friendMask, enemyMask, masks);
            }

            ourPawns &= ~(1ull << piecePosition);
        }
    }

    // STEP 3: construct boards for all moves which don't end with the king in check.
    // the order these are done in should be such that boards that are more likely to be better
    // are put into `result` first.
    vector<std::shared_ptr<BitBoard>> result;

    // get king non-capturing moves not involving castling
    applyAllKingMoves(result, kingMoves, kingPosition.position, friendMask, enemyMask, masks);

    // check for castling
    if (getCurrentPlayerOffset() == Player::White && enemyCheckingPieces.empty()) {
        if (isWhiteLeftCastleable() && checkCastleable(kingPosition.position, A1, friendMask, enemyMask, masks)) {
            auto newBoard = result.emplace_back(make_unique<BitBoard>(*this));
            newBoard->applyKingMove({kingPosition, kingPosition.position-2}); 
            newBoard->applyRookMove({A1, kingPosition.position-1}); 
            newBoard->swapCurrentPlayer();
        }
        if (isWhiteRightCastleable() && checkCastleable(kingPosition.position, H1, friendMask, enemyMask, masks)) {
            auto newBoard = result.emplace_back(make_unique<BitBoard>(*this));
            newBoard->applyKingMove({kingPosition, kingPosition.position+2}); 
            newBoard->applyRookMove({H1, kingPosition.position+1}); 
            newBoard->swapCurrentPlayer();
        }
    } else if (getCurrentPlayerOffset() == Player::Black && enemyCheckingPieces.empty()) {
        if (isBlackLeftCastleable() && checkCastleable(kingPosition.position, A8, friendMask, enemyMask, masks)) {
            auto newBoard = result.emplace_back(make_unique<BitBoard>(*this));
            newBoard->applyKingMove({kingPosition, kingPosition.position-2}); 
            newBoard->applyRookMove({A8, kingPosition.position-1});
            newBoard->swapCurrentPlayer();
        } 
        if (isBlackRightCastleable() && checkCastleable(kingPosition.position, H8, friendMask, enemyMask, masks)) {
            auto newBoard = result.emplace_back(make_unique<BitBoard>(*this));
            newBoard->applyKingMove({kingPosition, kingPosition.position+2}); 
            newBoard->applyRookMove({H8, kingPosition.position+1}); 
            newBoard->swapCurrentPlayer();
        }
    }

    // get pawn capturing moves that don't put the king in check
    applyAllPawnMoves(result, pawnCaptureMoves, kingPosition.position, friendMask, enemyMask, masks);

    // get bishop capturing moves that don't put the king in check
    applyAllBishopMoves(result, bishopCaptureMoves, kingPosition.position, friendMask, enemyMask, masks);

    // get knight capturing moves that don't put the king in check
    applyAllKnightMoves(result, knightCaptureMoves, kingPosition.position, friendMask, enemyMask, masks);

    // get rook capturing moves that don't put the king in check
    applyAllRookMoves(result, rookCaptureMoves, kingPosition.position, friendMask, enemyMask, masks);

    // get queen capturing moves that don't put the king in check
    applyAllQueenMoves(result, queenCaptureMoves, kingPosition.position, friendMask, enemyMask, masks);

    // get king capturing moves not involving castling
    applyAllKingMoves(result, kingCaptureMoves, kingPosition.position, friendMask, enemyMask, masks);

    // get queen moves that don't put the king in check
    applyAllQueenMoves(result, queenMoves, kingPosition.position, friendMask, enemyMask, masks);

    // get rook moves that don't put the king in check
    applyAllRookMoves(result, rookMoves, kingPosition.position, friendMask, enemyMask, masks);

    // get bishop moves that don't put the king in check
    applyAllBishopMoves(result, bishopMoves, kingPosition.position, friendMask, enemyMask, masks);

    // get knight moves that don't put the king in check
    applyAllKnightMoves(result, knightMoves, kingPosition.position, friendMask, enemyMask, masks);

    // get pawn moves that don't put the king in check
    applyAllPawnMoves(result, pawnMoves, kingPosition.position, friendMask, enemyMask, masks);


#ifdef DEBUG
    for (auto& m : result) {
        if (!m->pieces[Piece::K]) {
            std::cout << "WHITE KING MISSING" << std::endl;
            m->debugPrint();
        }
        if (!m->pieces[Piece::k]) {
            std::cout << "BLACK KING MISSING" << std::endl;
            m->debugPrint();
        }
    }
    
#endif

    return result;

}

BitBoard::BitBoard(const BitBoard& bitboard) : temporalInfo{bitboard.temporalInfo}
{
    std::copy(bitboard.pieces, bitboard.pieces + 12, this->pieces);
}

BoardStateSimplified BitBoard::getStateSimplified() const
{
    const U64 whiteMask = getPlayerCombinedPieceMask(Player::White);
    const U64 blackMask = getPlayerCombinedPieceMask(Player::Black);

    bool whiteInCheck = false;
    bool blackInCheck = false;
    if (getCurrentPlayerOffset() == Player::White) {
        whiteInCheck = isKingInCheck(std::countr_zero(pieces[Piece::K]), whiteMask, blackMask, Masks::getMasks());
    } else {
        blackInCheck = isKingInCheck(std::countr_zero(pieces[Piece::k]), blackMask, whiteMask, Masks::getMasks());
    }

    const int legalMoveCount = generateValidNextPositions().size();

    bool whiteHasWin = false;
    bool blackHasWin = false;
    bool hasDraw = false;

    if (getCurrentPlayerOffset() == Player::Black && legalMoveCount == 0) {
        // checkmate
        if (blackInCheck) {
            whiteHasWin = true;
        // stalemate
        } else {
            hasDraw = true;
        }
    } else if (getCurrentPlayerOffset() == Player::White && legalMoveCount == 0) {
        // checkmate
        if (whiteInCheck) {
            blackHasWin = true;
        // stalemate
        } else {
            hasDraw = true;
        }
    }

    // check for draws from insufficient material (not physically possible to checkmate):

    // king vs king
    if (std::popcount(whiteMask) == 1 && std::popcount(blackMask) == 1) {
        hasDraw = true;

    // king and (one of {knight, bishop}) vs king
    } else if ((std::popcount(whiteMask) + std::popcount(blackMask) == 3) 
                && (std::popcount(pieces[Piece::B]) + std::popcount(pieces[Piece::N]) 
                + std::popcount(pieces[Piece::b]) + std::popcount(pieces[Piece::n]) == 1)) {
        hasDraw = true;

    // king and bishop vs king and bishop, but both bishops are the same colour
    } else if (std::popcount(whiteMask) + std::popcount(blackMask) == 4 && std::popcount(pieces[Piece::B]) == 1 && std::popcount(pieces[Piece::b]) == 1
    && std::countr_zero(pieces[Piece::B]) % 2 == std::countr_zero(pieces[Piece::b]) % 2) {
        hasDraw = true;
    }

    return BoardStateSimplified(whiteHasWin, hasDraw, blackHasWin);
}

BoardState BitBoard::getState() const
{
    const std::vector<Piece> grid = getPieceList();
    const BoardStateSimplified simpleState = getStateSimplified();

    const U64 whiteMask = getPlayerCombinedPieceMask(Player::White);
    const U64 blackMask = getPlayerCombinedPieceMask(Player::Black);

    bool whiteInCheck = false;
    bool blackInCheck = false;
    if (getCurrentPlayerOffset() == Player::White) {
        whiteInCheck = isKingInCheck(std::countr_zero(pieces[Piece::K]), whiteMask, blackMask, Masks::getMasks());
    } else {
        blackInCheck = isKingInCheck(std::countr_zero(pieces[Piece::k]), blackMask, whiteMask, Masks::getMasks());
    }

    return BoardState(grid, Player(getCurrentPlayerOffset()), simpleState.whiteHasWin, simpleState.hasDraw, simpleState.blackHasWin, whiteInCheck, blackInCheck);
}

BoardState BitBoard::getStateSetup() const
{
    const std::vector<Piece> grid = getPieceList();
    return BoardState(grid, Player::White, false, false, false, false, false);
}

bool BitBoard::isValidMove(const Move& move) const
{
    std::vector<std::shared_ptr<BitBoard>> possibleNextBoards = generateValidNextPositions();
    BitBoard copy = BitBoard{*this};
    copy.makePlayerMove(move);
    for (auto board : possibleNextBoards) {
        if (copy == *board) {
            return true;
        }
    }
    return false;
}

void BitBoard::makePlayerMove(const Move& move)
{
    const int currentPlayersTurn = getCurrentPlayerOffset();
    const int pieceOffset = findPieceOffset(move.start.position, currentPlayersTurn);
    if (pieceOffset == Piece::K) {
        // white left castle
        if (move.start.position == E1 && move.end.position == C1) {
            applyKingMove({E1, C1}); 
            applyRookMove({A1, D1}); 
        // white right castle
        } else if (move.start.position == E1 && move.end.position == G1) {
            applyKingMove({E1, G1}); 
            applyRookMove({H1, F1}); 
        // black left castle
        } else if (move.start.position == E8 && move.end.position == C8) {
            applyKingMove({E8, C8}); 
            applyRookMove({A8, D8}); 
        // black right castle
        } else if (move.start.position == E8 && move.end.position == G8) {
            applyKingMove({E8, G8}); 
            applyRookMove({H8, F8}); 

        // normal king move
        } else {
            applyKingMove(move);
        }
    } else if (pieceOffset == Piece::Q) {
        applyQueenMove(move);
    } else if (pieceOffset == Piece::B) {
        applyBishopMove(move);
    } else if (pieceOffset == Piece::N) {
        applyKnightMove(move);
    } else if (pieceOffset == Piece::R) {
        applyRookMove(move);
    } else if (pieceOffset == Piece::P) {
        applyPawnMove(move);
    }
    
    swapCurrentPlayer();
}

Piece BitBoard::getPiece(const Position& position) const
{
    for (int i = 0; i < 12; ++i) {
        if (pieces[i] & (1ull << position.position)) {
            return Piece(i);
        }
    }

    // invalid piece, so find the colour of the chess board square
    if (Masks::checkerboardPattern & (1ull << position.position)) {
        return Piece::LightNone;
    } else {
        return Piece::DarkNone;
    }
}

void BitBoard::setPiece(const Position& position, const Piece& piece)
{
    pieces[piece] |= (1ull << position.position);
}

void BitBoard::removePiece(const Position& position)
{
    for (int piece = 0; piece < 12; ++piece) {
        pieces[piece] &= ~(1ull << position.position);
    }
}

bool BitBoard::validSetup() const
{
    // make sure there is exactly 1 of each king
    if (std::popcount(pieces[Piece::K]) != 1 || std::popcount(pieces[Piece::k]) != 1) {
        return false;
    }

    // make sure neither king is in check
    BoardState state = getState(); // assume that after every setup, white goes first
    if (state.blackInCheck || state.whiteInCheck) {
        return false;
    }

    // make sure all pawns are not in the first or last ranks
    const auto masks = Masks::getMasks();
    if (pieces[Piece::P] & masks->rankMasks[0] || pieces[Piece::P] & masks->rankMasks[H8] || pieces[Piece::p] & masks->rankMasks[0] || pieces[Piece::p] & masks->rankMasks[H8]) {
        return false;
    }

    return true;
}

bool BitBoard::operator==(const BitBoard& board) const
{
    bool same = true;
    for (int i = 0; i < 12; ++i) {
        same = same && (this->pieces[i] == board.pieces[i]);
    }
    return same && (this->temporalInfo == board.temporalInfo);
}

bool BitBoard::needsPawnPromotion() const
{
    const auto masks = Masks::getMasks();
    
    return (pieces[Piece::P] & masks->rankMasks[0] || pieces[Piece::P] & masks->rankMasks[H8] || pieces[Piece::p] & masks->rankMasks[0] || pieces[Piece::p] & masks->rankMasks[H8]);
}

void BitBoard::applyPawnPromotion(const Piece& newPiece)
{
    const auto masks = Masks::getMasks();

    // this is the pawn mask for the player that the piece promotion is being applied to
    const int pawnMask = (newPiece / Piece::k) + Piece::P;

    // this gets applicable pawns on the first or last rank (this keeps the logic simple,
    // so we don't need to worry about which player it is)
    const U64 getPromotedPawn = pieces[pawnMask] & (masks->rankMasks[0] | masks->rankMasks[H8]);
    const int promotionPosition = std::countr_zero(getPromotedPawn);

    // remove the pawn from the board
    pieces[pawnMask] &= ~(1ull << promotionPosition);
    pieces[newPiece] |= (1ull << promotionPosition);
}

BitBoard& BitBoard::operator=(const BitBoard& board)
{
    for (int i = 0; i < 12; ++i) {
        this->pieces[i] = board.pieces[i];
    }
    this->temporalInfo = board.temporalInfo;
    return *this;
}

void BitBoard::setBoardToBeEmpty()
{
    for (int i = 0; i < 12; ++i) {
        pieces[i] = 0ull;
    }
    temporalInfo = 0;
}

void BitBoard::manuallySetPlayerTurn(const Player player)
{
    if (getCurrentPlayerOffset() != player) {
        swapCurrentPlayer();
    }
}

//////////////////////////
// EVALUATION METHODS:
//////////////////////////

U64 BitBoard::pressureRay(const int position, int& bonus, const int incrementAmount, const U64 friendMask, const U64 enemyMask, const std::vector<U64>& masks) const
{
    const U64 combinedMask = friendMask | enemyMask;
    
    // cast ray
    for (auto mask : masks) {
        
        // this means there was a collision
        if (combinedMask & mask) {

            return combinedMask & mask;

        // if there was no collision
        } else {
            bonus += incrementAmount;
        }
    }
    return 0ull;
}

int BitBoard::getPieceScores(const Player p) const
{
    std::unordered_map<Piece, int> values = {
            {Piece::K, KING_VALUE}, {Piece::Q, QUEEN_VALUE}, {Piece::B, BISHOP_VALUE}, {Piece::N, KNIGHT_VALUE}, 
            {Piece::R, ROOK_VALUE}, {Piece::P, PAWN_VALUE}};

    int sumScore = 0;
    for (int i = 0; i < 6; ++i) {
        sumScore += std::popcount(pieces[p + i]) * values[Piece(i)];
    }
    return sumScore;
}

int BitBoard::queenPressureBonus(const Player p) const
{
    const auto masks = Masks::getMasks();
    int bonus = 0;
    const U64 friendMask = getPlayerCombinedPieceMask(p);
    const U64 enemyMask = getPlayerCombinedPieceMask(Player(6-p));
    const int enemyKingPosition = std::countr_zero(pieces[6-p]);
    U64 ourQueens = pieces[static_cast<int>(p) + Piece::Q];

    const int totalPieceCount = std::popcount(getPlayerCombinedPieceMask(Player::White) | getPlayerCombinedPieceMask(Player::Black));

    // loop through each queen
    while (ourQueens > 0) {
        const int queenPosition = std::countr_zero(ourQueens);

        // add on a bonus for the queen being open, and collect the squares its rays collide with

        U64 collisionSquares = 0ull;
        collisionSquares |= pressureRay(queenPosition, bonus, 1, friendMask, enemyMask, masks->attackRayRight[queenPosition]);
        collisionSquares |= pressureRay(queenPosition, bonus, 1, friendMask, enemyMask, masks->attackRayDiagonalUpRight[queenPosition]);
        collisionSquares |= pressureRay(queenPosition, bonus, 1, friendMask, enemyMask, masks->attackRayUp[queenPosition]);
        collisionSquares |= pressureRay(queenPosition, bonus, 1, friendMask, enemyMask, masks->attackRayDiagonalUpLeft[queenPosition]);
        collisionSquares |= pressureRay(queenPosition, bonus, 1, friendMask, enemyMask, masks->attackRayLeft[queenPosition]);
        collisionSquares |= pressureRay(queenPosition, bonus, 1, friendMask, enemyMask, masks->attackRayDiagonalDownLeft[queenPosition]);
        collisionSquares |= pressureRay(queenPosition, bonus, 1, friendMask, enemyMask, masks->attackRayDown[queenPosition]);
        collisionSquares |= pressureRay(queenPosition, bonus, 1, friendMask, enemyMask, masks->attackRayDiagonalDownRight[queenPosition]);
        
        // award position bonuses
        bonus += Masks::queenPositionBonus[p/6][queenPosition];

        // award points for each tile touching the enemy king (or the enemy king itself) that this queen could attack
        bonus += (50-totalPieceCount) * std::popcount(collisionSquares & masks->threeByThreeSquareMask[enemyKingPosition]);
        ourQueens &= ~(1ull << queenPosition);
    }

    return bonus;
}

int BitBoard::rookPressureBonus(const Player p) const
{
    const auto masks = Masks::getMasks();
    int bonus = 0;
    const U64 friendMask = getPlayerCombinedPieceMask(p);
    const U64 enemyMask = getPlayerCombinedPieceMask(Player(6-p));
    const int enemyKingPosition = std::countr_zero(pieces[6-p]);
    U64 ourRooks = pieces[static_cast<int>(p) + Piece::R];

    const int totalPieceCount = std::popcount(getPlayerCombinedPieceMask(Player::White) | getPlayerCombinedPieceMask(Player::Black));

    // loop through each rook
    while (ourRooks > 0) {
        const int rookPosition = std::countr_zero(ourRooks);

        // add on a bonus for the rook being open, and collect the squares its rays collide with

        U64 collisionSquares = 0ull;
        collisionSquares |= pressureRay(rookPosition, bonus, 1, friendMask, enemyMask, masks->attackRayRight[rookPosition]);
        collisionSquares |= pressureRay(rookPosition, bonus, 1, friendMask, enemyMask, masks->attackRayUp[rookPosition]);
        collisionSquares |= pressureRay(rookPosition, bonus, 1, friendMask, enemyMask, masks->attackRayLeft[rookPosition]);
        collisionSquares |= pressureRay(rookPosition, bonus, 1, friendMask, enemyMask, masks->attackRayDown[rookPosition]);
        
        // award position bonuses
        bonus += Masks::rookPositionBonus[p/6][rookPosition];

        // award points for each tile touching the enemy king (or the enemy king itself) that this rook could attack
        bonus += (40-totalPieceCount) * std::popcount(collisionSquares & masks->threeByThreeSquareMask[enemyKingPosition]);
        ourRooks &= ~(1ull << rookPosition);
    }

    return bonus;
}

int BitBoard::bishopPressureBonus(const Player p) const
{
    const auto masks = Masks::getMasks();
    int bonus = 0;
    const U64 friendMask = getPlayerCombinedPieceMask(p);
    const U64 enemyMask = getPlayerCombinedPieceMask(Player(6-p));
    const int enemyKingPosition = std::countr_zero(pieces[6-p]);
    U64 ourBishops = pieces[static_cast<int>(p) + Piece::Q];

    const int totalPieceCount = std::popcount(getPlayerCombinedPieceMask(Player::White) | getPlayerCombinedPieceMask(Player::Black));

    // loop through each bishop
    while (ourBishops > 0) {
        const int bishopPosition = std::countr_zero(ourBishops);

        // add on a bonus for the bishop being open, and collect the squares its rays collide with

        U64 collisionSquares = 0ull;
        collisionSquares |= pressureRay(bishopPosition, bonus, 1, friendMask, enemyMask, masks->attackRayDiagonalUpRight[bishopPosition]);
        collisionSquares |= pressureRay(bishopPosition, bonus, 1, friendMask, enemyMask, masks->attackRayDiagonalUpLeft[bishopPosition]);
        collisionSquares |= pressureRay(bishopPosition, bonus, 1, friendMask, enemyMask, masks->attackRayDiagonalDownLeft[bishopPosition]);
        collisionSquares |= pressureRay(bishopPosition, bonus, 1, friendMask, enemyMask, masks->attackRayDiagonalDownRight[bishopPosition]);
        
        // award position bonuses
        bonus += Masks::bishopPositionBonus[p/6][bishopPosition];

        // award points for each tile touching the enemy king (or the enemy king itself) that this bishop could attack
        bonus += (40-totalPieceCount) * std::popcount(collisionSquares & masks->threeByThreeSquareMask[enemyKingPosition]);
        ourBishops &= ~(1ull << bishopPosition);
    }

    return bonus;
}

int BitBoard::connectedRooksBonus(const Player p) const
{
    const auto masks = Masks::getMasks();
    int bonus = 0;
    const U64 friendMask = getPlayerCombinedPieceMask(p);
    const U64 enemyMask = getPlayerCombinedPieceMask(Player(6-p));
    U64 ourRooks = pieces[static_cast<int>(p) + Piece::R];

    // loop through each rook
    while (ourRooks > 0) {
        const int rookPosition = std::countr_zero(ourRooks);

        // add on a bonus for the rook being open, and collect the squares its rays collide with

        U64 collisionSquares = 0ull;
        collisionSquares |= pressureRay(rookPosition, bonus, 0, friendMask, enemyMask, masks->attackRayRight[rookPosition]);
        collisionSquares |= pressureRay(rookPosition, bonus, 0, friendMask, enemyMask, masks->attackRayUp[rookPosition]);
        collisionSquares |= pressureRay(rookPosition, bonus, 0, friendMask, enemyMask, masks->attackRayLeft[rookPosition]);
        collisionSquares |= pressureRay(rookPosition, bonus, 0, friendMask, enemyMask, masks->attackRayDown[rookPosition]);
        
        // award 15 points for each friendly rook that this rook collides with (so, each pair of rook collisions will add 30 points)
        bonus += 15 * std::popcount(collisionSquares & pieces[static_cast<int>(p) + Piece::R]);
        ourRooks &= ~(1ull << rookPosition);
    }

    return bonus;
}

int BitBoard::connectedAndStackedPawnBonus(const Player p) const
{
    const auto masks = Masks::getMasks();
    int bonus = 0;
    U64 ourPawns = pieces[static_cast<int>(p) + Piece::P];

    const int totalPieceCount = std::popcount(getPlayerCombinedPieceMask(Player::White) | getPlayerCombinedPieceMask(Player::Black));

    // loop through each pawn
    while (ourPawns > 0) {
        const int pawnPosition = std::countr_zero(ourPawns);

        //........
        //...101...
        //...1P1...   P == this pawn, 1 is good
        //...101...
        //.........
        const U64 goodNeighbourSquares = (masks->threeByThreeSquareMask[pawnPosition] & masks->fileNotMasks[pawnPosition]);

        // everything in the file except for this pawn itself
        const U64 badNeighbourSquares = masks->fileMasks[pawnPosition] & ~(1ull << pawnPosition);

        // don't care about this in the opening
        if (totalPieceCount > 30) {
            // add 5 points for any pawns 1 square away on the left or on the right
            bonus += 5 * std::popcount(goodNeighbourSquares & pieces[static_cast<int>(p) + Piece::P]);
        }


        // subtract points for stacked pawns, weighted more heavily towards endgame
        // late game this potentially could subtract ~60 points for 1 pair of stacked pawns
        bonus -= (32 - totalPieceCount) * std::popcount(badNeighbourSquares & pieces[static_cast<int>(p) + Piece::P]); 
        ourPawns &= ~(1ull << pawnPosition);
    }
    return bonus;
}

int BitBoard::activeKnights(const Player p) const
{
    const auto masks = Masks::getMasks();
    int bonus = 0;
    U64 ourKnights = pieces[static_cast<int>(p) + Piece::N];

    // loop through each pawn
    while (ourKnights > 0) {
        const int knightPosition = std::countr_zero(ourKnights);

        bonus += Masks::knightPositionBonus[p/6][knightPosition];
        ourKnights &= ~(1ull << knightPosition);
    }
    return bonus;
}

int BitBoard::pushedPawnsLateGame(const Player p) const
{
    const auto masks = Masks::getMasks();
    int bonus = 0;
    const U64 ourPawns = pieces[static_cast<int>(p) + Piece::P];

    const int totalPieceCount = std::popcount(getPlayerCombinedPieceMask(Player::White) | getPlayerCombinedPieceMask(Player::Black));

    // we only care about this later into a game
    if (totalPieceCount > 16) {
        return 0;
    }

    const int whiteValues[8] = {0, 0, 50, 75, 100, 150, 250, 0};
    const int blackValues[8] = {0, 250, 150, 100, 75, 50, 0, 0};

    // go through the 6 ranks where pawns could possibly be
    for (int rank = 1; rank < 7; ++rank) {
        const int pawnsOnThisRank = std::popcount(masks->rankMasks[rank*8] & ourPawns);
        bonus += pawnsOnThisRank * ((p == Player::White) ? whiteValues[rank] : blackValues[rank]);
    }
    return bonus;
}

int BitBoard::kingPositionEarlyGame(const Player p) const
{
    const int enemyPieceCount = std::popcount(getPlayerCombinedPieceMask(Player(6-p)));
    
    // if they have few potential attackers, we can be more aggressive, so don't add a bonus here
    if (enemyPieceCount < 9) {
        return 0;
    }

    const int kingPosition = std::countr_zero(pieces[p]);
    if (p == Player::White) {
        return Masks::whiteKingEarlyGame[kingPosition];
    } else {
        return Masks::blackKingEarlyGame[kingPosition];
    }
}

int BitBoard::mopupEvaluationBonus(const Player p) const
{
    const U64 enemyMask = getPlayerCombinedPieceMask(Player(6-p));

    // only activate if the enemy has a few pieces
    // AKA, don't activate if there are pawns on the board, or if the enemy has more than a few pieces
    if (std::popcount(enemyMask) > 3) {
        return 0;
    }

    // only activate if we have much more material than them
    const int friendlyNonKingMaterial = getPieceScores(p) - KING_VALUE;
    const int enemyNonKingMaterial = getPieceScores(Player(6-p)) - KING_VALUE;

    if (friendlyNonKingMaterial > 1.5 * enemyNonKingMaterial) { 
        return 0;
    }

    const int friendKingPosition = std::countr_zero(pieces[p]);
    const int enemyKingPosition = std::countr_zero(pieces[6-p]);

    // want to drive the losing king to the corners of the board, so use manhattan distance of it from the centre
    int bonus = 500 * Masks::centreManhattanDistance[enemyKingPosition];

    // also want our king to be close to theirs
    const int kingManhattanDistance = abs(enemyKingPosition/8 - friendKingPosition/8) + abs(enemyKingPosition%8 - friendKingPosition%8);
    bonus += 100 * (14 - kingManhattanDistance);

    // give a bonus for checking their king
    BoardState state = getState();
    if (kingManhattanDistance <= 4 && ((p == Player::White && state.blackInCheck) || (p == Player::Black && state.whiteInCheck))) {
        bonus += 1500;
    }
    
    return bonus;
}
