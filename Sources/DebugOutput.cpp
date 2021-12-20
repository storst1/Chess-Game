#include "DebugOutput.h"

extern const float FINF = 1e9f;

void D_PrintPosition(BOARD* _board){
    qDebug() << QString::fromStdString(_board->position_to_string()) << "\n";
}

void D_PrintEvalForPos_NewSearch(BOARD* _board){
    bNODE* main_node = new bNODE(_board);
    QPair<float, MOVE> best_branch = main_node->alphaBetaMax(-FINF, FINF, _DEPTH_);
    delete main_node;
    qDebug() << "Evaluation for pos is: " << best_branch.first << "\n";
}

void D_PrintEvalForPos_BasedOnDoneSearch(float eval){
    qDebug() << eval << "\n";
}
