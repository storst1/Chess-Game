#ifndef BETTERAI_H
#define BETTERAI_H

#include "board.h"
#include <future>

extern BOARD* board;
const int _INF = 1e9;
const int _DEPTH_ = 4;

MOVE search(BOARD* cur_board);

class bNODE{
protected:
    BOARD* _board;
public:
    bNODE(BOARD* _board):_board(_board){};
    QPair<float, MOVE> alphaBetaMax(float alpha, float beta, int depthLeft);
    QPair<float, MOVE> alphaBetaMin(float alpha, float beta, int depthLeft);
    QPair<float, MOVE> brute_force_search(int depthLeft, bool add_to_counter);
    float get_evaluation();
    void SortMoveList(QVector<MOVE>& MoveList);
    bNODE* create_new_node(MOVE move);
    ~bNODE(){
        if(_board != board) delete _board;
    };
};

#endif // BETTERAI_H
