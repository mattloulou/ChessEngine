#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include "BoardSubject.h"
#include "BitBoard.h"

class GameBoard: public BoardSubject {
    BitBoard board;
    bool setupMode;
public:
    GameBoard();
    ~GameBoard();

    // call right before a player makes a move for their turn
    BoardState getState() const;

    // returns if a move is allowed to be played in the current position
    bool isValidMove(const Move& move) const;
    
    // returns if a promotion move is allowed to be played in the current position
    bool isValidPromotionMove(const Move& move, const Piece& piece) const;

    // make a move for a human player
    // precondition: the move is valid
    void makePlayerMove(const Move& move);

    // make a promotion move for a human player
    // precondition: the move is valid
    void makePlayerPromotionMove(const Move& move, const Piece& piece);

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

    std::vector<std::shared_ptr<BitBoard>> generateValidNextPositions() const;

    // this removes all pieces, and sets all temporal information to 0 (disables castling rights and sets player to white)
    void setBoardToBeEmpty();

    // this sets the player's turn to the `player` parameter.
    void manuallySetPlayerTurn(const Player player);

    // retrieves the bitboard for the gameboard
    BitBoard getBitBoard();

    // sets the board to a new board
    void setBitBoard(BitBoard board);

    // resets the board to default
    void resetBoard();

    // sets setup mode
    void setSetup(bool inSetup);
};

#endif
