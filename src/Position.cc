#include "Position.h"

Position::Position(int position) : position{position} {}

Position::Position(int rank, int file) : position{rank*8 + file} {}

int Position::getRank() const
{
    return position / 8;
}

int Position::getFile() const
{
    return position % 8;
}
