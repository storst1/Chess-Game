#include "possible_moves_vector.h"
#include "board.h"

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
        if(check_info.first == DIR::KM && abs(move.piece) != 6 && move.ToCoords() != check_info.second){
            return;
        }
        if(board.ChecksAmount() == 1 && abs(move.piece) != 6 && check_info.first != DIR::KM){
            DIR dir_after_move_relative_to_threat = Direction::CalculateDir(move.ToCoords(), check_info.second);
            if(!(dir_after_move_relative_to_threat == DIR::NONE || dir_after_move_relative_to_threat == Direction::Opposite(check_info.first))){
                return;
            }
            DIR dir_after_move_relative_to_own_king = Direction::CalculateDir(move.ToCoords(), (board.Turn() ? board.KingPos(true) : board.KingPos(false)));
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
            DIR dir_after_move_relative_to_own_king = Direction::CalculateDir(move.ToCoords(), (board.Turn() ? board.KingPos(true) : board.KingPos(false)));
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
