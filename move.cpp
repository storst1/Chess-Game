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

Coords Move::FromCoords() const noexcept
{
    return Coords{x1, y1};
}

Coords Move::ToCoords() const noexcept
{
    return Coords{x2, y2};
}

PossibleMovesVector::PossibleMovesVector(Board &board) : board(board)
{

}

std::vector<Move>::const_iterator PossibleMovesVector::begin() const
{
    return moves.begin();
}

std::vector<Move>::const_iterator PossibleMovesVector::end() const
{
    return moves.end();
}

void PossibleMovesVector::push_back(Move move)
{
    if(move.piece == 6){
        if(board.CheckIfSquareDefended(move.x2, move.y2)){
            return;
        }
    }
    else{
        DIR move_dir = Direction::CalculateDir(move.FromCoords(), move.ToCoords());
        if(!board.CheckIfMoveIsAllowed(move.x1, move.y1, move_dir)){
            return;
        }
    }
    moves.push_back(move);
}

void PossibleMovesVector::reserve(size_t size)
{
    moves.reserve(size);
}

void PossibleMovesVector::clear()
{
    moves.clear();
}
