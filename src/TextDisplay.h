#ifndef TEXTDISPLAY_H
#define TEXTDISPLAY_H

#include "DisplayObserver.h"

class TextDisplay: public DisplayObserver {
    void showBoard(const std::vector<Piece> grid) override;
    void showCheck(const Player inCheck) override;
    void showWin(const Player winner) override;
    void showStalemate() override;
    void showDraw() override;
    void showResign(const Player winner) override;
    void showStandings() override;
public:
    TextDisplay(std::shared_ptr<GameBoard> gameBoard);
    ~TextDisplay();
};

#endif
