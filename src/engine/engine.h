#ifndef ENGINE_H
#define ENGINE_H

#include <chrono>

#include "board/board.h"
#include "piece_value.h"

class Engine
{
public:
    Engine(Board& board);
    ~Engine() = default;

    void SetDepth(uint_fast8_t new_depth);

    int EvaluatePosition() noexcept;
    Move BestNextMove() noexcept;
    double GetLastEvalTime() const noexcept;
private:
    std::pair<int, Move> AlphaBetaSearch(int alpha, int beta, uint_fast8_t depth_left) noexcept;
    int Quiesce(int alpha, int beta, bool initial_turn) noexcept;
    //int alphaBetaMax(int alpha, int beta, uint_fast8_t depth_left);
    //int alphaBetaMin(int alpha, int beta, uint_fast8_t depth_left);
private:
    Board board;
    uint_fast8_t depth = 7;
    PieceValue piece_value;
    Move default_move{9, 9, 9, 9, 9, 9};

    std::chrono::duration<double, std::milli> last_eval_time;

    const int INF = INT_MAX;
};

#endif // ENGINE_H
