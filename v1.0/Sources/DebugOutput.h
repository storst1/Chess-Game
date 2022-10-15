#ifndef DEBUGOUTPUT_H
#define DEBUGOUTPUT_H

#include "board.h"
#include "BetterAI.h"

void D_PrintPosition(BOARD* _board);
void D_PrintEvalForPos_NewSearch(BOARD* _board);
void D_PrintEvalForPos_BasedOnDoneSearch(float eval);

#endif // DEBUGOUTPUT_H
