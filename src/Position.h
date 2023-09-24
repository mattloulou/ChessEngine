#ifndef POSITION_H
#define POSITION_H


struct Position {
    int position; // rank = position/8, file = position%8

    Position(int position);
    Position(int rank, int file);
    int getRank() const;
    int getFile() const;
};

#endif
