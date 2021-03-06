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

PossibleMovesVector::PossibleMovesVector(Board &board) : board(board)
{

}

PossibleMovesVector::PossibleMovesVector(const PossibleMovesVector &other) noexcept : moves(other.moves), board(other.board)
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

std::vector<Move>::iterator PossibleMovesVector::begin()
{
    return moves.begin();
}

std::vector<Move>::iterator PossibleMovesVector::end()
{
    return moves.end();
}

void PossibleMovesVector::push_back(Move move)
{
    if(board.IsCheck()){
        if(board.ChecksAmount() > 1 && abs(move.piece) != 6){
            return;
        }

        std::pair<DIR, Coords>& check_info = board.GetCheckByIdx(0);
        if(check_info.first == DIR::KM && abs(move.piece) != 6){
            return;
        }
        if(board.ChecksAmount() == 1 && abs(move.piece) != 6){
            DIR dir_after_move_relative_to_threat = Direction::CalculateDir(move.ToCoords(), check_info.second);
            if(!(dir_after_move_relative_to_threat == DIR::NONE || dir_after_move_relative_to_threat == Direction::Opposite(check_info.first))){
                return;
            }
            DIR dir_after_move_relative_to_own_king = Direction::CalculateDir(move.ToCoords(), (board.Turn() ? board.FindKing(true) : board.FindKing(false)));
            if(dir_after_move_relative_to_own_king == Direction::Opposite(check_info.first)){
                return;
            }
        }
    }

    if(abs(move.piece) == 6){
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

void PossibleMovesVector::push_back(Move &move)
{
    if(board.IsCheck()){
        if(board.ChecksAmount() > 1 && abs(move.piece) != 6){
            return;
        }

        std::pair<DIR, Coords>& check_info = board.GetCheckByIdx(0);
        if(check_info.first == DIR::KM && abs(move.piece) != 6){
            return;
        }
        if(board.ChecksAmount() == 1 && abs(move.piece) != 6){
            DIR dir_after_move_relative_to_threat = Direction::CalculateDir(move.ToCoords(), check_info.second);
            if(!(dir_after_move_relative_to_threat == DIR::NONE || dir_after_move_relative_to_threat == Direction::Opposite(check_info.first))){
                return;
            }
            DIR dir_after_move_relative_to_own_king = Direction::CalculateDir(move.ToCoords(), (board.Turn() ? board.FindKing(true) : board.FindKing(false)));
            if(dir_after_move_relative_to_own_king == Direction::Opposite(check_info.first)){
                return;
            }
        }
    }

    if(abs(move.piece) == 6){
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

size_t PossibleMovesVector::size() const noexcept
{
    return moves.size();
}

bool PossibleMovesVector::empty() const noexcept
{
    return moves.empty();
}
