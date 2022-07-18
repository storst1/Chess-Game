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

int Engine::AlphaBetaSearch(int alpha, int beta, uint_fast8_t depth_left) noexcept
{
    if(depth_left == 0){
        return EvaluatePosition();
    }
    for(const auto& cur_move : board.PossibleMovesRef()){
        board.RunMove(cur_move);
        int score = -AlphaBetaSearch(-beta, -alpha, depth_left - 1);
        board.RevertLastMove();
        if(score >= beta){
            return beta;
        }
        if(score > alpha){
            alpha = score;
        }
    }
    return alpha;
}
