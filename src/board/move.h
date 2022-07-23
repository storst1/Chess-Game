#ifndef MOVE_H
#define MOVE_H

#include <vector>
#include <cstdint>

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

    inline Coords FromCoords() const noexcept;
    inline Coords ToCoords() const noexcept;

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

class PossibleMovesVector{
public:
    PossibleMovesVector(Board& board);
    PossibleMovesVector(const PossibleMovesVector& other) noexcept;
    ~PossibleMovesVector() = default;

    std::vector<Move>::const_iterator begin() const;
    std::vector<Move>::const_iterator end() const;
    std::vector<Move>::iterator begin();
    std::vector<Move>::iterator end();

    void push_back(Move move);
    void push_back(Move& move);
    void reserve(size_t size);
    void clear();
    size_t size() const noexcept;
    bool empty() const noexcept;
private:
    std::vector<Move> moves;
    Board& board;
};

#endif // MOVE_H
