#ifndef ENGINE_H
#define ENGINE_H

#include "board.h"
#include "piece_value.h"

class Engine
{
public:
    Engine(Board& board);
    ~Engine() = default;

    void SetDepth(uint_fast8_t new_depth);

    int EvaluatePosition() noexcept;
private:
    int AlphaBetaSearch(int alpha, int beta, uint_fast8_t depth_left) noexcept;
private:
    Board board;
    uint_fast8_t depth = 8;
    PieceValue piece_value;
};

#endif // ENGINE_H
