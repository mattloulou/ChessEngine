#include "ChessController.h"
#include "GameBoard.h"
#include "Game.h"
#include "DisplayObserver.h"
#include "TextDisplay.h"
#include "FancyTextDisplay.h"
#include <sstream>
#include <unordered_map>

unordered_map<char, Piece> pieceMap = {
    {'K', Piece::K}, {'Q', Piece::Q}, {'B', Piece::B}, {'N', Piece::N}, 
    {'R', Piece::R}, {'P', Piece::P}, {'k', Piece::k}, {'q', Piece::q}, 
    {'b', Piece::b}, {'n', Piece::n}, {'r', Piece::r}, {'p', Piece::p}};

unordered_map<char, Piece> promotionPieceMap = {
    {'Q', Piece::Q}, {'B', Piece::B}, {'N', Piece::N}, {'R', Piece::R},
    {'q', Piece::q}, {'b', Piece::b}, {'n', Piece::n}, {'r', Piece::r}};

void ChessController::runController() 
{    
    std::shared_ptr<GameBoard> board = std::make_shared<GameBoard>();
    Player currentTurn = Player::White; 
    Game game{board, currentTurn, 0, 0};
    bool isWhiteHuman = false;
    bool isBlackHuman = false;
    bool setGame = false;
    bool setupSet = false;

    std::string line;
    
    shared_ptr<DisplayObserver> display = std::make_shared<FancyTextDisplay>(board);
    board->attach(display);
    
    while (std::getline(std::cin, line)) {
        if (setGame && game.isGameOver()) {
            setGame = false;
            board->resetBoard();
            isWhiteHuman = false;
            isBlackHuman = false;
            currentTurn = Player::White;
        }
        std::string cmd;
        std::string str1;
        std::string str2;

        std::stringstream ss(line);
        std::getline(ss, cmd, ' ');

        if (cmd == "game") {
            // check for the number of arguments
            if (numberArg(line) != 3) {
                std::cout << "Invalid number of arguments to start a new game." << std::endl;
                continue;
            }
            std::getline(ss, str1, ' ');

            // reset the game if it is already set 
            // note that will not be custom setup since a game is already running
            if (!setupSet) {
                board->resetBoard();
                isWhiteHuman = false;
                isBlackHuman = false;
                currentTurn = Player::White; 
            }
            setupSet = false;

            // white-player
            int whiteType = 0;
            if (str1 == "human") {
                isWhiteHuman = true;
            } else if (str1 == "computer1") {
                whiteType = 1;
            } else if (str1 == "computer2") {
                whiteType = 2;
            } else if (str1 == "computer3") {
                whiteType = 3;
            } else if (str1 == "computer4") {
                whiteType = 4;
            } else {
                std::cout << "Invalid input for white-player to start a new game." << std::endl;
                continue;
            }

            // black-player
            int blackType = 0;
            std::getline(ss, str2, ' ');
            if (str2 == "human") {
                isBlackHuman = true;
            } else if (str2 == "computer1") {
                blackType = 1;
            } else if (str2 == "computer2") {
                blackType = 2;
            } else if (str2 == "computer3") {
                blackType = 3;
            } else if (str2 == "computer4") {
                blackType = 4;
            } else {
                std::cout << "Invalid input for black-player to start a new game." << std::endl;
                continue;
            }
            game = Game{board, currentTurn, whiteType, blackType};
            board->notifyObservers(); // notify the display 
            setGame = true;
        } else if (cmd == "resign") {
            if (numberArg(line) != 1) {
                std::cout << "Too many arguments to resign." << std::endl;
                continue;
            }
            if (!setGame) {
                std::cout << "No game in progress." << std::endl;
                continue;
            }
            display->notifyResign(currentTurn);
            game.updateScore();
            board->resetBoard();
            setGame = false;
        } else if (cmd == "move") {
            if (!setGame) {
                std::cout << "No game in progress." << std::endl;
                continue;
            }

            std::getline(ss, str1, ' ');
            std::getline(ss, str2, ' ');

            // check if it is the computer's turn 
            if ((currentTurn == Player::White && !isWhiteHuman) || (currentTurn == Player::Black && !isBlackHuman)) {
                if (numberArg(line) == 1) {
                    game.makeNextMoveComputer();
                    if (game.isGameOver()) {
                        game.updateScore();
                    }
                    // the Player::White and Player::Black enum differ by 6 to accomodate for the piece offsets in the bitboard
                    currentTurn = Player(6 - currentTurn); 
            
                    currentTurn = playAutomatic(game, currentTurn, isWhiteHuman, isBlackHuman); // play computer automatically if compiler flag is set
                } else {
                    std::cout << "Invalid number of arguments. It is a computer's turn." << std::endl;
                }
                continue;
            }

            // check for the number of arguments
            if (numberArg(line) != 3 && numberArg(line) != 4) {
                std::cout << "Invalid number of arguments. It is a player's turn, please input two positions." << std::endl;
                continue;
            }
            
            // check if the positions are valid
            if (!isValidPosition(str1) || !isValidPosition(str2)) {
                std::cout << "Invalid input. It is a player's turn, please input valid positions." << std::endl;
                continue;
            }

            const int moveStart = (std::toupper(str1[0]) - 'A') + 8 * (str1[1] - '1');
            const int moveEnd = (std::toupper(str2[0]) - 'A') + 8 * (str2[1] - '1');

            if (numberArg(line) == 3) {
                // check if the move is valid and make the move
                if (!game.isValidMove({moveStart, moveEnd})) {
                    std::cout << "Invalid move. ";
                    if (currentTurn == Player::White) std::cout << "White, try again." << std::endl;
                    else std::cout << "Black, try again." << std::endl;
                    continue;
                } else {
                    game.makeNextMoveHuman({moveStart, moveEnd});
                }
            } else {
                // we have a promotion given, so we need to proceed differently
                std::string str3;
                std::getline(ss, str3, ' ');
                Piece piece;
                if (promotionPieceMap.count(str3[0])) {
                    piece = pieceMap[str3[0]];
                } else {
                    std::cout << "Invalid promotion piece." << std::endl;
                    continue;
                }
                // if it is not valid tell the user, otherwise make the promotion move
                if (!game.isValidPromotionMove({moveStart, moveEnd}, piece)) {
                    std::cout << "Invalid promotion move. ";
                    if (currentTurn == Player::White) std::cout << "White, try again." << std::endl;
                    else std::cout << "Black, try again." << std::endl;
                    continue;
                } else {
                    game.makeNextPromotionMoveHuman({moveStart, moveEnd}, piece);
                }
            }
            if (game.isGameOver()) {
                game.updateScore();
            }
            currentTurn = Player(6 - currentTurn);
            currentTurn = playAutomatic(game, currentTurn, isWhiteHuman, isBlackHuman); // play computer automatically if compiler flag is set
        } else if (cmd == "setup") {
            // check for the number of arguments
            if (numberArg(line) != 1) {
                std::cout << "Too many arguments to setup a board." << std::endl;
                continue;
            }

            if (setGame) {
                std::cout << "Cannot setup a game while it is running." << std::endl;
                continue;
            }

            // tell the board we are in setup mode
            board->setSetup(true);
                
            std::string setupline;
            cout << "Setup mode in progress" << endl;
            board->setBoardToBeEmpty(); // clear the board
            while (std::getline(std::cin, setupline)) {
                std::string cmd2;

                std::stringstream setupss(setupline);
                std::getline(setupss, cmd2, ' ');

                if (cmd2 == "+") {
                    // check for the number of arguments
                    if (numberArg(setupline) != 3) {
                        std::cout << "Invalid number of arguments to add a piece." << std::endl;
                        continue;
                    }

                    std::getline(setupss, str1, ' ');
                    std::getline(setupss, str2, ' ');

                    // check if the input is valid
                    if (str1.length() != 1 || pieceMap.count(str1[0]) == 0) {
                        std::cout << "Invalid piece." << std::endl;
                        continue;
                    }
                    const Piece piece = pieceMap[str1[0]];
                    
                    if (!isValidPosition(str2)) {
                        std::cout << "Invalid position." << std::endl;
                        continue;
                    }
                    Position pos = {str2[1] - '1', std::toupper(str2[0]) - 'A'};

                    // check if the position is occupied
                    if (board->getPiece(pos) != Piece::DarkNone && board->getPiece(pos) != Piece::LightNone) {
                        std::cout << "Position is already occupied." << std::endl;
                        continue;
                    }

                    // add the piece to the board
                    board->setPiece(pos, piece);

                    cout << "Added " << str1 << " at " << str2 << endl;
                } else if (cmd2 == "-") {
                    // check for the number of arguments
                    if (numberArg(setupline) != 2) {
                        std::cout << "Invalid number of arguments to remove a piece." << std::endl;
                        continue;
                    }

                    std::getline(setupss, str1, ' ');
                    if (!isValidPosition(str1)) {
                        std::cout << "Invalid position." << std::endl;
                        continue;
                    }

                    Position pos = {str1[1] - '1', std::toupper(str1[0]) - 'A'};
                    if (board->getPiece(pos) == Piece::DarkNone || board->getPiece(pos) == Piece::LightNone) {
                        std::cout << "No piece at this position to be removed." << std::endl;
                        continue;
                    }
                    board->removePiece(pos);
                } else if (cmd2 == "=") {
                    // check for the number of arguments
                    if (numberArg(setupline) != 2) {
                        std::cout << "Invalid number of arguments to set the colour." << std::endl;
                        continue;
                    }

                    std::getline(setupss, str1, ' ');

                    for (auto & c : str1) {
                        c = std::tolower(c); // to support both upper and lower case
                    }

                    if (str1 == "white") {
                        currentTurn = Player::White;
                        board->manuallySetPlayerTurn(Player::White);
                    } else if (str1 == "black") {
                        currentTurn = Player::Black;
                        board->manuallySetPlayerTurn(Player::Black);
                    } else {
                        std::cout << "Invalid colour. Please input White or Black." << std::endl;
                        continue;
                    }
                } else if (cmd2 == "done") {
                    // check for the number of arguments
                    if (numberArg(setupline) != 1) {
                        std::cout << "Too many arguments to finish setup." << std::endl;
                        continue;
                    }

                    if (board->validSetup()) {
                        // tell the board we are leaving setup mode
                        board->setSetup(false);
                        setupSet = true;
                        break;
                    } else {
                        std::cout << "Invalid setup." << std::endl;
                    }
                } else {
                    std::cout << "Invalid command, try again. You are currently in setup mode." << std::endl;
                }
            }
        } else {
            std::cout << "Invalid command, try again." << std::endl;
        }
    }
    endProgram(*board); // print final score
    board->detach(display);
    return;
}

void ChessController::endProgram(GameBoard& board) 
{
    board.notifyObserversQuit(); // print final score
    return;
}

// set computers to play automatically
void ChessController::setAutomatic() 
{
    automatic = true;
}

Player ChessController::playAutomatic(Game& game, int currentTurn, bool isWhiteHuman, bool isBlackHuman)
{
    while (!game.isGameOver() && automatic && ((currentTurn == Player::White && !isWhiteHuman) || (currentTurn == Player::Black && !isBlackHuman))) {
        game.makeNextMoveComputer();
        currentTurn = Player(6 - currentTurn);
    }
    return Player(currentTurn);
}

// check if the position is valid in an 8x8 board
bool ChessController::isValidPosition(const string position) const {
    if (position.length() != 2) {
        return false;
    }
    const char file = std::toupper(position[0]);
    const char rank = position[1];

    if (file < 'A' || file > 'H' || rank < '1' || rank > '8') {
        return false;
    }

    return true;
}

int ChessController::numberArg(const string& line) const {
    std::stringstream ss(line);
    std::string arg;

    int count = 0;
    while (ss >> arg) {
        ++count;
    }
    return count;
}
