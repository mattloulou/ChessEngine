#ifndef GAME_H
#define GAME_H

#include "Move.h"
#include "Player.h"
#include "GameBoard.h"
#include "Computer.h"
#include <map>

class Game {
    // the current game board for the game
    std::shared_ptr<GameBoard> gameBoard;
    // the current player whose turn it is in the game
    Player currPlayer;
    // the white player's computer (exists iff white is a computer player)
    std::unique_ptr<Computer> whiteComputer;
    // the black player's computer (exists iff black is a computer player)
    std::unique_ptr<Computer> blackComputer;
    // the score of the white player across all games
    static float whiteScore;
    // the score of the black player across all games
    static float blackScore;
public:
    Game(std::shared_ptr<GameBoard> gameBoard, Player currentPlayer, int whiteType, int blackType);
    Game& operator=(const Game& other);

    // checks if a given move is valid for the game state
    bool isValidMove(const Move move) const;
    // checks if a give promotion move is valid for the game state
    bool isValidPromotionMove(const Move move, const Piece piece) const;
    // makes the next move assuming it is a human's turn and the move is valid (already checked)
    void makeNextMoveHuman(const Move move);
    // makes the next move assuming it is a human's turn and the move is valid and it is a promotion move
    void makeNextPromotionMoveHuman(const Move move, const Piece piece);
    // makes the next move assuming it is a computer's turn and the move is valid (already checked)
    void makeNextMoveComputer();
    // checks if the game is over
    bool isGameOver() const;
    // updates the score based on the state of the game being over
    void updateScore();
    // gets the final white/black score
    static std::map<Player, float> getScore();
};

#endif
