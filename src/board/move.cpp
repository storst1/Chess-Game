#include "board.h"
#include "move.h"

Move::Move(int_fast8_t piece,
           uint_fast8_t x1,
           uint_fast8_t y1,
           uint_fast8_t x2,
           uint_fast8_t y2,
           int_fast8_t captured,
           uint_fast8_t ep_x,
           uint_fast8_t ep_y,
           bool castle) noexcept
        : piece(piece), x1(x1), y1(y1), x2(x2), y2(y2), captured(captured), ep_x(ep_x), ep_y(ep_y), castle(castle)
{

}

Move::Move(const Move &other) noexcept :
    piece(other.piece),
    x1(other.x1),
    y1(other.y1),
    x2(other.x2),
    y2(other.y2),
    captured(other.captured),
    ep_x(other.ep_x),
    ep_y(other.ep_y),
    castle(other.castle)
{

}

Move &Move::operator=(const Move &other) noexcept
{
    piece = other.piece;
    x1 = other.x1;
    y1 = other.y1;
    x2 = other.x2;
    y2 = other.y2;
    captured = other.captured;
    ep_x = other.ep_x;
    ep_y = other.ep_y;
    castle = other.castle;
    return *this;
}

Coords Move::FromCoords() const noexcept
{
    return Coords{x1, y1};
}

Coords Move::ToCoords() const noexcept
{
    return Coords{x2, y2};
}
