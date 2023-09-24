#include "DisplayObserver.h"
#include "GameBoard.h"

DisplayObserver::DisplayObserver(std::shared_ptr<GameBoard> gameBoard) : gameBoard{gameBoard} {}

void DisplayObserver::notify()
{ 
    BoardState state = gameBoard->getState();
    showBoard(state.grid); // print the updated board

    // print which player's turn it is
    if (state.playerTurn == Player::White) {
        std::cout << "White's Turn:" << std::endl;
    } else {
        std::cout << "Black's Turn:" << std::endl;
    }

    // display win, draw, or check, if needed
    if (state.whiteHasWin) {
        showWin(Player::White);
    } else if (state.blackHasWin) {
        showWin(Player::Black);
    } else if (state.hasDraw) {
        if (gameBoard->generateValidNextPositions().size() == 0) {
            showStalemate();
        } else {
            showDraw();
        }
    } else if (state.whiteInCheck) {
        showCheck(Player::White);
    } else if (state.blackInCheck) {
        showCheck(Player::Black);
    } 
}

void DisplayObserver::notifyQuit()
{
    showStandings();
}

void DisplayObserver::notifyResign(Player resignedPlayer) 
{
    showResign(resignedPlayer);
}

DisplayObserver::~DisplayObserver() {}
