#include "engine.h"

Engine::Engine(Board &board) : board(board), piece_value(":/piece_value.json")
{
    //zobrist_hash.Init();
    //zobrist_hash.SaveAsJsonFile(Zobrist_Hash::DefaultZHPath());
    zobrist_hash.LoadFromJsonFile(Zobrist_Hash::DefaultZHPath());
    //piece_value.D_PrintPieceValues();
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
    auto time_start = std::chrono::high_resolution_clock::now();
    auto result = AlphaBetaSearch(-INF, INF, depth);
    //auto result = NegamaxAlphaBetaSearch(-INF, INF, Engine::depth);
    auto time_end = std::chrono::high_resolution_clock::now();
    last_eval_time = time_end - time_start;
    qDebug() << "Minimax: " << result.first;
    //qDebug() << "Negamax: " << result.first;
    return result.second;
}

double Engine::GetLastEvalTime() const noexcept
{
    return last_eval_time.count();
}

constexpr int Engine::Inf() noexcept
{
    return INT_MAX;
}

//NegaMax variation
std::pair<int, Move> Engine::AlphaBetaSearch(int alpha, int beta, uint_fast8_t depth_left) noexcept
{
    if(depth_left == 0){

        return {(board.Turn() ? EvaluatePosition() : -EvaluatePosition()), default_move};
        //return {Quiesce(alpha, beta, initial_color), default_move};
    }

    bool moves_empty = board.PossibleMovesRef().size() == 0;
    if(moves_empty){
        //Check for checkmate and stalemate and return
    }

    Move best_move = *board.PossibleMovesRef().begin();
    int best = -INF;

    for(const Move& move : board.PossibleMoves()){
        board.RunMove(move);
        auto[score, cur_best_move] = AlphaBetaSearch(-beta, -alpha, depth_left - 1);
        score = -score;
        board.RevertLastMove();
        if(score > best){
            best = score;
            best_move = move;
        }
        if(best > alpha){
            alpha = best;
        }
        if(best >= beta){
            break;
        }
    }
    return {best, best_move};
}

std::pair<int, Move> Engine::NegamaxAlphaBetaSearch(int alpha, int beta, uint_fast8_t depth_left) noexcept
{
    if(depth_left == 0){
        return {(board.Turn() ? EvaluatePosition() : -EvaluatePosition()), default_move};
    }

    int value = -INF;
    Move best_move = *board.PossibleMovesRef().begin();
    for(const Move& move : board.PossibleMoves()){
        board.RunMove(move);
        auto[score, best_move] = NegamaxAlphaBetaSearch(-beta, -alpha, depth_left - 1);
        board.RevertLastMove();

        if(-score > value){
            value = -score;
            best_move = move;
        }
        alpha = std::max(alpha, value);
        if(alpha >= beta){
            break; //cut-off
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
