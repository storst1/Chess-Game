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
Move &Engine::BestNextMove() noexcept
{
    return AlphaBetaSearch(-INF, INF, depth, board.Turn()).second;
}

std::pair<int, Move &> Engine::AlphaBetaSearch(int alpha, int beta, uint_fast8_t depth_left, bool initial_turn) noexcept
{
    if(depth_left == 0){
        return {Quiesce(alpha, beta), default_move};
    }

    Move& best_move = *board.PossibleMovesRef().begin();
    if(board.Turn() == initial_turn){
        int value = -INF;
        for(const auto& move : board.PossibleMovesRef()){
            board.RunMove(move);
            int score = AlphaBetaSearch(alpha, beta, depth_left - 1, initial_turn).first;
            if(score > value){
                value = score;
                best_move = move;
            }
            if(value >= beta){
                break;
            }
            alpha = std::max(alpha, value);
        }
        return {value, best_move};
    }
    else{
        int value = INF;
        for(const auto& move : board.PossibleMovesRef()){
            board.RunMove(move);
            int score = AlphaBetaSearch(alpha, beta, depth_left - 1, initial_turn).first;
            if(score < value){
                value = score;
                best_move = move;
            }
            if(value <= alpha){
                break;
            }
            beta = std::min(beta, value);
        }
        return {value, best_move};
    }
}

int Engine::Quiesce(int alpha, int beta) noexcept
{
    int standing_pat = EvaluatePosition();
    if(standing_pat >= beta){
        return beta;
    }
    if(alpha < standing_pat){
        alpha = standing_pat;
    }

    for(const auto& move : board.PossibleMovesRef()){
        //Do not consider and skip non-capture moves
        if(!(move.captured != 0 || move.ep_x != 8)){
            continue;
        }

        board.RunMove(move);
        int score = -Quiesce(-beta, -alpha);

        if(score >= beta){
            return beta;
        }
        if(score > alpha){
            alpha = score;
        }

        board.RevertLastMove();
    }
    return alpha;
}
