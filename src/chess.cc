#include "ChessController.h"
#include "BitBoard.h"
#include "ChessSquares.h"
#include "Computer.h"
#include <iostream>
#include <fstream>
#include <memory>
#include <cstring>

int main(int argc, char *argv[]) 
{
    srand(time(0));
    ChessController c;
    for (int i = 0; i < argc; ++i) {
        if (std::strcmp(argv[i], "--auto") == 0) {
            c.setAutomatic();
        }
    }
    c.runController();
}
