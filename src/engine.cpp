#include "engine.h"

Engine::Engine(Board &board) : board(board)
{

}

void Engine::SetDepth(uint_fast8_t new_depth)
{
    depth = new_depth;
}

int Engine::EvaluatePosition() noexcept
{
    int score = 0;
    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            int_fast8_t cur_value = board.Get(i, j);
            score += ((cur_value > 0) ? piece_value.GetPieceValue(cur_value) : -piece_value.GetPieceValue(cur_value));
        }
    }
    return score;
}

//Assumes that there're possible moves in given position
Move Engine::BestNextMove() noexcept
{
    return AlphaBetaSearch(-INF, INF, depth, board.Turn()).second;
}

std::pair<int, Move> Engine::AlphaBetaSearch(int alpha, int beta, uint_fast8_t depth_left, bool initial_turn) noexcept
{
    if(depth_left == 0){
        return {Quiesce(alpha, beta, initial_turn), default_move};
    }

    Move best_move = *board.PossibleMovesRef().begin();
    int value;
    if(board.Turn() == initial_turn){
        value = -INF;
        for(const auto& move : board.PossibleMoves()){
            board.RunMove(move);
            int score = AlphaBetaSearch(alpha, beta, depth_left - 1, initial_turn).first;
            board.RevertLastMove();
            if(score > value){
                value = score;
                best_move = move;
            }
            if(value >= beta){
                break;
            }
            alpha = std::max(alpha, value);
        }
    }
    else{
        value = INF;
        for(const auto& move : board.PossibleMoves()){
            board.RunMove(move);
            int score = AlphaBetaSearch(alpha, beta, depth_left - 1, initial_turn).first;
            board.RevertLastMove();
            if(score < value){
                value = score;
                best_move = move;
            }
            if(value <= alpha){
                break;
            }
            beta = std::min(beta, value);
        }
    }
    return {value, best_move};
}

int Engine::Quiesce(int alpha, int beta, bool initial_turn) noexcept
{
    bool further_positions = false;
    int value;
    if(board.Turn() == initial_turn){
        value = -INF;
        for(const auto& move : board.PossibleMoves()){
            //Do not consider and skip non-capture moves
            if(move.captured == 0 || move.ep_x == 8){
                continue;
            }

            further_positions = true;

            board.RunMove(move);
            int score = Quiesce(alpha, beta, initial_turn);
            board.RevertLastMove();
            if(score > value){
                value = score;
            }
            if(value >= beta){
                break;
            }
            alpha = std::max(alpha, value);
        }
    }
    else{
        value = INF;
        for(const auto& move : board.PossibleMoves()){
            //Do not consider and skip non-capture moves
            if(move.captured == 0 || move.ep_x == 8){
                continue;
            }

            further_positions = true;

            board.RunMove(move);
            int score = Quiesce(alpha, beta, initial_turn);
            board.RevertLastMove();
            if(score < value){
                value = score;
            }
            if(value <= alpha){
                break;
            }
            beta = std::min(beta, value);
        }
    }

    return (further_positions ? value : EvaluatePosition());
}
