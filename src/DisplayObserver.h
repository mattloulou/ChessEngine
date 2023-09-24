#ifndef DISPLAYOBSERVER_H
#define DISPLAYOBSERVER_H

#include <memory>
#include <vector>
#include "Piece.h"
#include "Player.h"

class GameBoard;

class DisplayObserver {
    std::shared_ptr<GameBoard> gameBoard;

    virtual void showBoard(const std::vector<Piece> grid) = 0;
    virtual void showCheck(const Player inCheck) = 0;
    virtual void showWin(const Player winner) = 0;
    virtual void showStalemate() = 0;
    virtual void showDraw() = 0; // there are other ways to draw besides stalemate
    virtual void showResign(const Player winner) = 0;
    virtual void showStandings() = 0;
public: 
    DisplayObserver(std::shared_ptr<GameBoard> gameBoard);
    
    void notify(); // notified by GameBoard - concrete BoardSubject
    void notifyQuit(); // notified by ChessController
    void notifyResign(Player resignedPlayer);
    virtual ~DisplayObserver();
};

#endif
