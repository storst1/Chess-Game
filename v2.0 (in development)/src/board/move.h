#ifndef MOVE_H
#define MOVE_H

#include <vector>
#include <cstdint>

#include <QString>

class Board;
struct Coords;

struct Move
{
    Move(int_fast8_t piece,
         uint_fast8_t x1,
         uint_fast8_t y1,
         uint_fast8_t x2,
         uint_fast8_t y2,
         int_fast8_t captured,
         uint_fast8_t ep_x = 8,
         uint_fast8_t ep_y = 8,
         bool castle = false
         ) noexcept;
    Move(const Move& other) noexcept;
    ~Move() = default;

    Move& operator=(const Move& other) noexcept;

    Coords FromCoords() const noexcept;
    Coords ToCoords() const noexcept;
    QString ToString() const noexcept;

    int_fast8_t piece;
    uint_fast8_t x1;
    uint_fast8_t y1;
    uint_fast8_t x2;
    uint_fast8_t y2;
    int_fast8_t captured; //value of the (x2, y2) before the move, this value is not valid only in case of en passant
    uint_fast8_t ep_x; //X coords of captured pawn by en passant
    uint_fast8_t ep_y; //Y coords of captured pawn by en passant
    bool castle; //Determines if move was a castle move
};

#endif // MOVE_H
