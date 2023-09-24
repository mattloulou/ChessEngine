#ifndef FANCYTEXTDISPLAY_H
#define FANCYTEXTDISPLAY_H

#include "DisplayObserver.h"

class FancyTextDisplay: public DisplayObserver {
    void showBoard(const std::vector<Piece> grid) override;
    void showCheck(const Player inCheck) override;
    void showWin(const Player winner) override;
    void showStalemate() override;
    void showDraw() override;
    void showResign(const Player resignee) override;
    void showStandings() override;
public:
    FancyTextDisplay(std::shared_ptr<GameBoard> gameBoard);
    ~FancyTextDisplay();
};

#endif
