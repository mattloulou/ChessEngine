#include "FancyTextDisplay.h"
#include "GameBoard.h"
#include "Game.h"
#include <unordered_map>

FancyTextDisplay::FancyTextDisplay(std::shared_ptr<GameBoard> gameBoard) : DisplayObserver{gameBoard} {}

FancyTextDisplay::~FancyTextDisplay() {}

void FancyTextDisplay::showBoard(const std::vector<Piece> grid)
{
    std::unordered_map<Piece,std::string> pieceMap = {
            {Piece::K," ♔"}, {Piece::Q," ♕"}, {Piece::B," ♗"}, {Piece::N," ♘"}, 
            {Piece::R," ♖"}, {Piece::P," ♙"}, {Piece::k," ♚"}, {Piece::q," ♛"}, 
            {Piece::b," ♝"}, {Piece::n," ♞"}, {Piece::r," ♜"}, {Piece::p," ♟︎"},
            {Piece::DarkNone,"⬛"}, {Piece::LightNone,"⬜"}};
    
    // set up the board with tiles
    std::vector<std::string> boardOutput;
    for (const auto& p : grid) {
        boardOutput.push_back(pieceMap[p]);
    }

    // print the board 
    for (int rank = 7; rank >= 0; --rank) {
        std::cout << (rank + 1) << " ";
        for (int file = 0; file < 8; ++file) {
            std::cout << boardOutput[rank * 8 + file];
        }
        std::cout << std::endl;
    }
    std::cout << "   a b c d e f g h" << endl;
}

void FancyTextDisplay::showCheck(const Player inCheck)
{
    if (inCheck == Player::White) {
        std::cout << "White is in check." << std::endl;
    } else {
        std::cout << "Black is in check." << std::endl;
    }
}

void FancyTextDisplay::showWin(const Player winner)
{
    if (winner == Player::White) {
        std::cout << "Checkmate! White wins!" << endl;
    } else {
        std::cout << "Checkmate! Black wins!" << endl;
    }
}

void FancyTextDisplay::showStalemate()
{
    std::cout << "Stalemate!" << std::endl;
}

void FancyTextDisplay::showDraw()
{
    std::cout << "Draw!" << std::endl;
}

void FancyTextDisplay::showResign(const Player resignee)
{
    if (resignee == Player::White) {
        std::cout << "Black wins!" << endl;
    } else {
        std::cout << "White wins!" << endl;
    }
}

void FancyTextDisplay::showStandings()
{
    std::cout << "Final Score:" << std::endl;
    std::cout << "White: " << Game::getScore()[Player::White] << std::endl;
    std::cout << "Black: " << Game::getScore()[Player::Black] << std::endl; 
}
