#include "GameBoard.h"

GameBoard::GameBoard() : board{ BitBoard() } {}

GameBoard::~GameBoard() {}

BoardState GameBoard::getState() const
{
    if (setupMode) {
        return board.getStateSetup();
    } else {
        return board.getState();
    }
}

bool GameBoard::isValidMove(const Move& move) const
{
    return board.isValidMove(move);
}

bool GameBoard::isValidPromotionMove(const Move& move, const Piece& piece) const
{
    // get a copy of our current bit board
    BitBoard currBoard = board;
    // get valid next moves
    std::vector<std::shared_ptr<BitBoard>> nextBoards = currBoard.generateValidNextPositions();
    // make the move
    currBoard.makePlayerMove(move);
    // if the piece is valid set the piece for promotion
    currBoard.removePiece(move.end);
    currBoard.setPiece(move.end, piece);
    // check if the result is valid
    bool validPromotionMove = false;
    for (auto& option : nextBoards) {
        if (*option == currBoard) {
            validPromotionMove = true;
            break;
        }
    }
    return validPromotionMove;
}

void GameBoard::makePlayerMove(const Move& move)
{
    board.makePlayerMove(move);
    notifyObservers();
}

void GameBoard::makePlayerPromotionMove(const Move& move, const Piece& piece)
{
    board.makePlayerMove(move);
    board.removePiece(move.end);
    board.setPiece(move.end, piece);
    notifyObservers();
}

Piece GameBoard::getPiece(const Position& position) const
{
    return board.getPiece(position);
}

void GameBoard::setPiece(const Position& position, const Piece& piece)
{
    board.setPiece(position, piece);
    notifyObservers();
}

void GameBoard::removePiece(const Position& position)
{
    board.removePiece(position);
    notifyObservers();
}

bool GameBoard::validSetup() const
{
    return board.validSetup();
}

std::vector<std::shared_ptr<BitBoard>> GameBoard::generateValidNextPositions() const
{
    return board.generateValidNextPositions();
}

void GameBoard::setBoardToBeEmpty()
{
    board.setBoardToBeEmpty();
}

void GameBoard::manuallySetPlayerTurn(const Player player)
{
    board.manuallySetPlayerTurn(player);
}

BitBoard GameBoard::getBitBoard()
{
    return board;
}

void GameBoard::setBitBoard(BitBoard board)
{
    this->board = board;
    notifyObservers();
}

void GameBoard::resetBoard()
{
    board = BitBoard();
}

void GameBoard::setSetup(bool inSetup)
{
    setupMode = inSetup;
}
