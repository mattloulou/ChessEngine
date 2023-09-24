#include "TextDisplay.h"
#include "GameBoard.h"
#include "Game.h"
#include <unordered_map>

TextDisplay::TextDisplay(std::shared_ptr<GameBoard> gameBoard) : DisplayObserver{gameBoard} {}

TextDisplay::~TextDisplay() {}

void TextDisplay::showBoard(const std::vector<Piece> grid) 
{
    std::unordered_map<Piece,std::string> pieceMap = {
            {Piece::K,"K"}, {Piece::Q,"Q"}, {Piece::B,"B"}, {Piece::N,"N"}, 
            {Piece::R,"R"}, {Piece::P,"P"}, {Piece::k,"k"}, {Piece::q,"q"}, 
            {Piece::b,"b"}, {Piece::n,"n"}, {Piece::r,"r"}, {Piece::p,"p"},
            {Piece::DarkNone,"_"}, {Piece::LightNone," "}};
    
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
    std::cout << "  abcdefgh" << endl;
}

void TextDisplay::showCheck(const Player inCheck) 
{
    if (inCheck == Player::White) {
        std::cout << "White is in check." << std::endl;
    } else {
        std::cout << "Black is in check." << std::endl;
    }
}

void TextDisplay::showWin(const Player winner) 
{
    if (winner == Player::White) {
        std::cout << "Checkmate! White wins!" << endl;
    } else {
        std::cout << "Checkmate! Black wins!" << endl;
    }
}

void TextDisplay::showStalemate()
{
    std::cout << "Stalemate!" << std::endl;
}

void TextDisplay::showDraw()
{
    std::cout << "Draw!" << std::endl;
}

void TextDisplay::showResign(const Player resignee)
{
    if (resignee == Player::White) {
        std::cout << "Black wins!" << endl;
    } else {
        std::cout << "White wins!" << endl;
    }
}

void TextDisplay::showStandings()
{
    std::cout << "Final Score:" << std::endl;
    std::cout << "White: " << Game::getScore()[Player::White] << std::endl;
    std::cout << "Black: " << Game::getScore()[Player::Black] << std::endl; 
}
