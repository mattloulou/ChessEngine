#include "Game.h"

float Game::whiteScore = 0.0;
float Game::blackScore = 0.0;

Game::Game(std::shared_ptr<GameBoard> gameBoard, Player currPlayer, int whiteType, int blackType) : gameBoard{gameBoard}, currPlayer{currPlayer}
{
    // assign the computers as needed based on level
    if (whiteType == 1) {
        whiteComputer = std::make_unique<Computer>(Player::White, 1);
    } else if (whiteType == 2) {
        whiteComputer = std::make_unique<Computer>(Player::White, 2);
    } else if (whiteType == 3) {
        whiteComputer = std::make_unique<Computer>(Player::White, 3);
    } else if (whiteType == 4) {
        whiteComputer = std::make_unique<Computer>(Player::White, 4);
    }
    if (blackType == 1) {
        blackComputer = std::make_unique<Computer>(Player::Black, 1);
    } else if (blackType == 2) {
        blackComputer = std::make_unique<Computer>(Player::Black, 2);
    } else if (blackType == 3) {
        blackComputer = std::make_unique<Computer>(Player::Black, 3);
    } else if (blackType == 4) {
        blackComputer = std::make_unique<Computer>(Player::Black, 4);
    }
}

Game& Game::operator=(const Game& other)
{
    gameBoard = other.gameBoard;
    currPlayer = other.currPlayer;
    if (other.whiteComputer != nullptr) {
        whiteComputer = std::make_unique<Computer>(*(other.whiteComputer));
    }
    if (other.blackComputer != nullptr) {
        blackComputer = std::make_unique<Computer>(*(other.blackComputer));
    }
    return *this;
}

bool Game::isValidMove(const Move move) const
{
    return gameBoard->isValidMove(move);
}

bool Game::isValidPromotionMove(const Move move, const Piece piece) const
{
    return gameBoard->isValidPromotionMove(move, piece);
}

void Game::makeNextMoveHuman(const Move move)
{
    gameBoard->makePlayerMove(move);
    if (currPlayer == Player::White) {
        currPlayer = Player::Black;
    } else {
        currPlayer = Player::White;
    }
}

void Game::makeNextPromotionMoveHuman(const Move move, const Piece piece)
{
    gameBoard->makePlayerPromotionMove(move, piece);
    if (currPlayer == Player::White) {
        currPlayer = Player::Black;
    } else {
        currPlayer = Player::White;
    }
}

void Game::makeNextMoveComputer()
{
    if (currPlayer == Player::White) {
        gameBoard->setBitBoard(*whiteComputer->findMove(gameBoard->getBitBoard()));
        currPlayer = Player::Black;
    } else {
        gameBoard->setBitBoard(*blackComputer->findMove(gameBoard->getBitBoard()));
        currPlayer = Player::White;
    }
}

bool Game::isGameOver() const
{
    BoardState currState = gameBoard->getState();
    return currState.whiteHasWin || currState.blackHasWin || currState.hasDraw;
}

void Game::updateScore()
{
    // if there is no win or draw, we assume the current player resigned so the other player gets the win
    BoardState currState = gameBoard->getState();
    if (currState.whiteHasWin) {
        Game::whiteScore += 1;
    } else if (currState.blackHasWin) {
        Game::blackScore += 1;
    } else if (currState.hasDraw) {
        Game::whiteScore += 0.5;
        Game::blackScore += 0.5;
    } else {
        if (currPlayer == Player::White) {
            Game::blackScore += 1;
        } else {
            Game::whiteScore += 1;
        }
    }
}

std::map<Player, float> Game::getScore()
{
    std::map<Player, float> scores {
        {Player::White, Game::whiteScore},
        {Player::Black, Game::blackScore}
    };

    return scores;
}
