#include "BetterAI.h"
#include "DebugOutput.h"

const float FINF = 1e9f;
int positions_evaluated;

extern float last_move_eval_time;

MOVE search(BOARD* cur_board){
    positions_evaluated = 0;
    auto start = std::chrono::steady_clock::now();
    bNODE* main_node = new bNODE(cur_board);
    QPair<float, MOVE> best_branch = main_node->alphaBetaMax(-FINF, FINF, _DEPTH_);
    D_PrintEvalForPos_BasedOnDoneSearch(best_branch.first);
    //QPair<float, MOVE> best_branch = main_node->brute_force_search(_DEPTH_); <- BRUTE FORCE METHOD
    delete main_node;
    auto end = std::chrono::steady_clock::now();
    double time_passed_in_sec = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 1000.0;
    qDebug() << positions_evaluated << " positions evaluated in " << time_passed_in_sec << " seconds \n";
    last_move_eval_time = time_passed_in_sec;
    return best_branch.second;
}

float bNODE::get_evaluation(){
    //return _board->get_evaluation();
    return _board->BETTER_EVALUATION();
}

QPair<float, MOVE> bNODE::alphaBetaMax(float alpha, float beta, int depthLeft){
    positions_evaluated++;
    if(depthLeft == 0 || _board->BETTER_game_status() != 1){
        return {get_evaluation(), MOVE()};
    }
    QVector<MOVE> all_moves = _board->get_all_possible_moves(_board->get_turn());
    if(depthLeft == _DEPTH_){
        SortMoveList(all_moves);
    }
    int i_alpha = 0;
    for(int i = 0; i < (int)all_moves.size(); ++i){
        bNODE* NEW_CUR_NODE = create_new_node(all_moves[i]);
        float score = NEW_CUR_NODE->alphaBetaMin(alpha, beta, depthLeft - 1).first;
        if(score >= beta){
            delete NEW_CUR_NODE;
            return {beta, all_moves[i]};
        }
        if(score > alpha){
            alpha = score;
            i_alpha = i;
        }
        delete NEW_CUR_NODE;
    }
    return {alpha, all_moves[i_alpha]};
}

QPair<float, MOVE> bNODE::alphaBetaMin(float alpha, float beta, int depthLeft){
    positions_evaluated++;
    if(depthLeft == 0 || _board->BETTER_game_status() != 1){
        return {-get_evaluation(), MOVE()};
    }
    QVector<MOVE> all_moves = _board->get_all_possible_moves(_board->get_turn());
    int i_beta = 0;
    for(int i = 0; i < (int)all_moves.size(); ++i){
        bNODE* NEW_CUR_NODE = create_new_node(all_moves[i]);
        float score = NEW_CUR_NODE->alphaBetaMax(alpha, beta, depthLeft - 1).first;
        if(score <= alpha){
            delete NEW_CUR_NODE;
            return {alpha, all_moves[i]};
        }
        if(score < beta){
            beta = score;
            i_beta = i;
        }
        delete NEW_CUR_NODE;
    }
    return {beta, all_moves[i_beta]};
}

void bNODE::SortMoveList(QVector<MOVE>& MoveList){
    const int BRUTE_FORCE_DEPTH = 1;
    int MoveListSize = MoveList.size();
    QVector<QPair<float, int>> Evaluations(MoveListSize);
    for(int i = 0; i < MoveListSize; ++i){
        bNODE* NEW_CUR_NODE = create_new_node(MoveList[i]);
        float cur_eval = NEW_CUR_NODE->brute_force_search(BRUTE_FORCE_DEPTH, false).first;
        Evaluations[i] = {cur_eval, i};
    }
    std::sort(Evaluations.begin(), Evaluations.end());
    QVector<MOVE> NewMoveList(MoveListSize);
    for(int i = MoveListSize - 1; i > -1; --i){
        NewMoveList[MoveListSize - i - 1] = MoveList[Evaluations[i].second];
    }
    MoveList = NewMoveList;
}

QPair<float, MOVE> bNODE::brute_force_search(int depthLeft, bool add_to_counter){
    if(add_to_counter)
        positions_evaluated++;
    if(depthLeft == 0 || _board->BETTER_game_status() != 1){
        return {get_evaluation(), MOVE()};
    }
    QVector<MOVE> all_moves = _board->get_all_possible_moves(_board->get_turn());
    int i_best = 0;
    float best = -FINF;
    for(int i = 0; i < (int)all_moves.size(); ++i){
        bNODE* NEW_CUR_NODE = create_new_node(all_moves[i]);
        /*
        std::future<float> future_score = std::async(std::launch::async, [](bNODE* NEW_CUR_NODE, int depthLeft){ return NEW_CUR_NODE->brute_force_search(depthLeft).first; }, NEW_CUR_NODE, depthLeft);
        float score = future_score.get();
        */
        float score = NEW_CUR_NODE->brute_force_search(depthLeft - 1, false).first;
        if(score > best){
            best = score;
            i_best = i;
        }
        delete NEW_CUR_NODE;
    }
    return {best, all_moves[i_best]};
}

bNODE *bNODE::create_new_node(MOVE move){
    BOARD* new_board = _board->transform_to_next_position(_board, move, true, true /*this should be set to false*/);
    bNODE* new_node = new bNODE(new_board);
    return new_node;
}
