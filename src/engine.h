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
    Move BestNextMove() noexcept;
private:
    std::pair<int, Move> AlphaBetaSearch(int alpha, int beta, uint_fast8_t depth_left, bool initial_turn) noexcept;
    int Quiesce(int alpha, int beta, bool initial_turn) noexcept;
private:
    Board board;
    uint_fast8_t depth = 5;
    PieceValue piece_value;
    Move default_move{9, 9, 9, 9, 9, 9};

    const int INF = INT_MAX;
};

#endif // ENGINE_H
