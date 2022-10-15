#ifndef DIRECTION_H
#define DIRECTION_H

#define DIR Direction::Dir

#include <cstdint>

#include "coords.h"

class Direction{
public:
    enum Dir{U, UR, R, DR, D, DL, L, UL, KM, NONE};
    static DIR CalculateDir(Coords from, Coords to) noexcept;
    static DIR Opposite(const Dir& dir) noexcept;
};

#endif // DIRECTION_H
