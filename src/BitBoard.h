#ifndef BITBOARD_H
#define BITBOARD_H

#include <cstdint>
#include <vector>
#include <unordered_set>
#include "BoardState.h"
#include "Move.h"
#include "Masks.h"

// Evaluation decorator forward declarations
class Level2;
class Level3;
class Level4Base;
class ActiveKnights;
class BishopPressure;
class EarlyGameKingPosition;
class QueenPressure;
class RookPressure;
class ConnectedRooks;
class PawnlessEndgame;
class PawnRelations;
class PushPawnsLateGame;
class Hash;

using U64 = uint64_t; 

class BitBoard {
    
    // A U64 mask represents the position of the pieces.
    // Each index in the array is in the same order as in the `Piece` enum.
    // the values set here are the default values for a chess board
    U64 pieces[12] = {0x0000000000000010ull, 0x0000000000000008ull, 0x0000000000000024ull,  // white King, Queen, Bishop
                      0x0000000000000042ull, 0x0000000000000081ull, 0x000000000000FF00ull,  // white Knight, Rook, Pawn
                      0x1000000000000000ull, 0x0800000000000000ull, 0x2400000000000000ull,  // black King, Queen, Bishop
                      0x4200000000000000ull, 0x8100000000000000ull, 0x00FF000000000000ull}; // black Knight, Rook, Pawn

    // bits 0-7 store if White's pawns just did a double-move (moved two squares from the second rank)
    // bits 8-15 store if Black's pawns just did a double-move (moved two squares from the second rank)
    // bit 16 stores if the bottom left rook has not moved (White)
    // bit 17 stores if the bottom right rook has not moved (White)
    // bit 18 stores if the top right rook has not moved (Black)
    // bit 19 stores if the top left rook has not moved (Black)
    // bit 20 stores if the White king has not moved
    // bit 21 stores the Black king has not moved
    // bit 22 stores the player's turn; 0 is white, 1 is black.
    uint32_t temporalInfo = 0x3F0000u;

    // METHODS FOR TEMPORAL INFORMATION

    inline bool isWhitePawnEnpassantable(const unsigned file) const;
    inline bool isBlackPawnEnpassantable(const unsigned file) const;
    inline void setWhitePawnEnpassantable(const unsigned file);
    inline void setBlackPawnEnpassantable(const unsigned file);
    inline void resetEnpassantFlags();

    // castleable methods check if the king and chosen rook are allowed to move,
    // but you must still check on your own that there is nothing in between them, and that the kings 
    // current, middle, and final positions are not in check.
    inline bool isWhiteLeftCastleable() const;
    inline bool isWhiteRightCastleable() const;
    inline bool isBlackLeftCastleable() const;
    inline bool isBlackRightCastleable() const;
    


    inline void setWhiteKingMoved();
    inline void setBlackKingMoved();
    inline void setWhiteLeftRookMoved();
    inline void setWhiteRightRookMoved();
    inline void setBlackLeftRookMoved();
    inline void setBlackRightRookMoved();

    inline int getCurrentPlayerOffset() const;
    inline int getOtherPlayerOffset() const;
    inline void swapCurrentPlayer();


    // METHODS FOR MOVE APPLICATION

    inline int findPieceOffset(const Position position, const int otherPlayerOffset) const;
    inline void movePiece(U64& tile, const Move& m);
    inline void applyKingMove(const Move& move);
    inline void applyQueenMove(const Move& move);
    inline void applyBishopMove(const Move& move);
    inline void applyKnightMove(const Move& move);
    inline void applyRookMove(const Move& move);
    inline void applyPawnMove(const Move& move);

    // METHODS FOR LEGAL MOVE GENERATION

    inline U64 getPlayerCombinedPieceMask(const Player player) const;

    // takes in a position on the board and a vector of masks (one of the ray masks from the Masks singleton), and casts that ray to find candidate moves
    inline static void useRayMask(std::vector<Move>& nonCaptureMoves, std::vector<Move>& captureMoves, const Position position, const U64 friendMask, const U64 enemyMask, const std::vector<U64>& masks);

    // finds the places a bishop could move. It does not take into account checks. Adds the moves to the given `moves` vector.
    inline static void candidateBishopMoves(std::vector<Move>& nonCaptureMoves, std::vector<Move>& captureMoves, const Position position, const U64 friendMask, const U64 enemyMask, shared_ptr<Masks> masks);

    // finds the places a rook could move. It does not take into account checks. Adds the moves to the given `moves` vector.
    inline static void candidateRookMoves(std::vector<Move>& nonCaptureMoves, std::vector<Move>& captureMoves, const Position position, const U64 friendMask, const U64 enemyMask, shared_ptr<Masks> masks);

    // finds the places a queen could move. It does not take into account checks. Adds the moves to the given `moves` vector.
    inline static void candidateQueenMoves(std::vector<Move>& nonCaptureMoves, std::vector<Move>& captureMoves, const Position position, const U64 friendMask, const U64 enemyMask, shared_ptr<Masks> masks);

    // finds the places a pawn could move. It does not take into account checks. Adds the moves to the given `moves` vector.
    inline void candidatePawnMoves(std::vector<Move>& nonCaptureMoves, std::vector<Move>& captureMoves, const Position position, const U64 friendMask, const U64 enemyMask, const U64 attackMask, const U64 moveMask) const;

    // finds the places a WHITE pawn could move. It does not take into account checks. Adds the moves to the given `moves` vector.
    inline void candidateWhitePawnMoves(std::vector<Move>& nonCaptureMoves, std::vector<Move>& captureMoves, const Position position, const U64 friendMask, const U64 enemyMask, shared_ptr<Masks> masks) const;

    // finds the places a BLACK pawn could move. It does not take into account checks. Adds the moves to the given `moves` vector.
    inline void candidateBlackPawnMoves(std::vector<Move>& nonCaptureMoves, std::vector<Move>& captureMoves, const Position position, const U64 friendMask, const U64 enemyMask, shared_ptr<Masks> masks) const;

    // finds the places a knight could move. It does not take into account checks. Adds the moves to the given `moves` vector.
    inline static void candidateKnightMoves(std::vector<Move>& nonCaptureMoves, std::vector<Move>& captureMoves, const Position position, const U64 friendMask, const U64 enemyMask, shared_ptr<Masks> masks);

    // finds the places a king could move. It does not take into account checks. Adds the moves to the given `moves` vector.
    inline static void candidateKingMoves(std::vector<Move>& nonCaptureMoves, std::vector<Move>& captureMoves, const Position position, const U64 friendMask, const U64 enemyMask, shared_ptr<Masks> masks);

    // like useRayMask, but gets the first enemy piece putting you in check
    // `nonhostileMask` means friendly pieces, and enemy pieces that can't attack for whatever reason (such as if you are casting a ray to a right,
    // then an enemy bishop couldn't harm you)
    inline static void checkRayForCheck(std::vector<Position>& enemyChecks, const U64 nonhostileMask, const U64 attackingEnemyMask, const std::vector<U64>& masks);

    inline void checkForKnightChecks(std::vector<Position>& enemyChecks, const Position position, const U64 enemyMask, shared_ptr<Masks> masks) const;
    inline void checkForHorizontalVerticalChecks(std::vector<Position>& enemyChecks, const Position position, const U64 friendMask, const U64 enemyMask, shared_ptr<Masks> masks) const;
    inline void checkForDiagonalChecks(std::vector<Position>& enemyChecks, const Position position, const U64 friendMask, const U64 enemyMask, shared_ptr<Masks> masks) const;
    inline void checkForEnemyKingChecks(std::vector<Position>& enemyChecks, const Position yourKingPosition, shared_ptr<Masks> masks) const;

    inline bool isKingInCheck(const Position kingPosition, const U64 friendMask, const U64 enemyMask, shared_ptr<Masks> masks) const;

    inline bool checkCastleable(const int kingPosition, const int rookPosition, const U64 friendMask, const U64 enemyMask, std::shared_ptr<Masks> masks) const;

    std::vector<Piece> getPieceList() const;

    inline void applyAllPawnMoves(std::vector<std::shared_ptr<BitBoard>>& result, std::vector<Move>& moves, const int kingPosition, const U64 friendMask, const U64 enemyMask, std::shared_ptr<Masks> masks) const;
    inline void applyAllBishopMoves(std::vector<std::shared_ptr<BitBoard>>& result, std::vector<Move>& moves, const int kingPosition, const U64 friendMask, const U64 enemyMask, std::shared_ptr<Masks> masks) const;
    inline void applyAllKnightMoves(std::vector<std::shared_ptr<BitBoard>>& result, std::vector<Move>& moves, const int kingPosition, const U64 friendMask, const U64 enemyMask, std::shared_ptr<Masks> masks) const;
    inline void applyAllRookMoves(std::vector<std::shared_ptr<BitBoard>>& result, std::vector<Move>& moves, const int kingPosition, const U64 friendMask, const U64 enemyMask, std::shared_ptr<Masks> masks) const;
    inline void applyAllQueenMoves(std::vector<std::shared_ptr<BitBoard>>& result, std::vector<Move>& moves, const int kingPosition, const U64 friendMask, const U64 enemyMask, std::shared_ptr<Masks> masks) const;
    inline void applyAllKingMoves(std::vector<std::shared_ptr<BitBoard>>& result, std::vector<Move>& moves, const int kingPosition, const U64 friendMask, const U64 enemyMask, std::shared_ptr<Masks> masks) const;



    // METHODS FOR EVALUATION PURPOSES:

    // increments bonus by increment amount for each empty square the ray is cast to, and returns the mask of the first
    // piece the ray collides with
    U64 pressureRay(const int position, int& bonus, const int incrementAmount, const U64 friendMask, const U64 enemyMask, const std::vector<U64>& masks) const;

    // basic piece scores:
    // King +1000000, Queen +800, Rook +500, Bishop/Knight +300, Pawn +100
    int getPieceScores(const Player p) const;

    // gives a positive bonus related to how many free tiles these pieces could move to, and also how many tiles near the king they can 
    // move to or capture
    int queenPressureBonus(const Player p) const;
    int rookPressureBonus(const Player p) const;
    int bishopPressureBonus(const Player p) const;

    // gives a positive bonus for having rooks in contact with each other
    int connectedRooksBonus(const Player p) const;

    // gives a positive bonus for pawns being near each other.
    // gives a deduction for having stacked pawns
    int connectedAndStackedPawnBonus(const Player p) const;

    // gives a positive bonus for having knights near the middle
    int activeKnights(const Player p) const;

    // gives a bonus for having pushed pawns late game
    int pushedPawnsLateGame(const Player p) const;

    // early game, kings get a bonus for being safe away from the board centre. Late game, kings get a bonus for being away from it.
    int kingPositionEarlyGame(const Player p) const;
    int kingPositionLateGame(const Player p) const;

    // mop-up evaluation; used to help end a game when one side clearly won
    // occurs when there are no pawns left on the board, and few pieces
    // gives a bonus for pushing the enemy king to the edge of the board to allow for easier checkmate
    int mopupEvaluationBonus(const Player p) const;

    friend Level2;
    friend Level3;
    friend Level4Base;
    friend ActiveKnights;
    friend BishopPressure;
    friend EarlyGameKingPosition;
    friend QueenPressure;
    friend RookPressure;
    friend ConnectedRooks;
    friend PawnRelations;
    friend PushPawnsLateGame;
    friend PawnlessEndgame;
    friend Hash;
public:

    BitBoard() = default;
    BitBoard(const BitBoard& bitboard);
    BitBoard(BitBoard&& bitboard);
    
    BitBoard& operator=(const BitBoard& bitboard);

    // call right before a player makes a move for their turn.
    // returns a concise list of info that evaluation needs to know
    BoardStateSimplified getStateSimplified() const;
    // returns all essential info external implementations need to know
    BoardState getState() const;
    // returns all essential info in setup mode
    BoardState getStateSetup() const;

    // returns if a move is allowed to be played in the current position
    bool isValidMove(const Move& move) const;

    // make a move for a human player
    // precondition: the move is valid
    void makePlayerMove(const Move& move);

    // get which piece is at a position
    Piece getPiece(const Position& position) const;

    // set the piece at a position
    // note that `piece` should be the correct piece for the correct colour
    // precondition: there should be no piece at this location
    void setPiece(const Position& position, const Piece& piece);

    // remove the piece at a position
    void removePiece(const Position& position);

    // Check if a setup is valid according to the project specifications.
    // There are real positions in chess that are valid despite this function saying they are not,
    // such as a position where a piece is in check.
    bool validSetup() const;

    // print out the board along with temporal info
    void debugPrint() const;

    bool needsPawnPromotion() const;

    // precondition: `piece` must be one of:
    // {Piece::Q, Piece::B, Piece::N, Piece::R,
    //  Piece::q, Piece::b, Piece::n, Piece::r}
    void applyPawnPromotion(const Piece& newPiece);

    bool operator==(const BitBoard& board) const;
    std::vector<std::shared_ptr<BitBoard>> generateValidNextPositions() const;

    // this removes all pieces, and sets all temporal information to 0 (disables castling rights and sets player to white)
    void setBoardToBeEmpty();

    // this sets the player's turn to the `player` parameter.
    void manuallySetPlayerTurn(const Player player);
};

#endif
