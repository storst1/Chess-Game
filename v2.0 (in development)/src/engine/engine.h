#ifndef ENGINE_H
#define ENGINE_H

#include <chrono>

#include "board/board.h"
#include "piece_value.h"
#include "zobrist_hash.h"
#include "perft.h"

class Engine
{
public:
    explicit Engine(Board& board);
    ~Engine() = default;

    void SetDepth(uint_fast8_t new_depth);

    int EvaluatePosition() noexcept;
    Move BestNextMove() noexcept;
    double GetLastEvalTime() const noexcept;

    //Perft
    bool PerformPerftCheck(const uint_fast8_t search_depth, const QString& correct_ans_json_path) const;

    static constexpr int Inf() noexcept;
private:
    std::pair<int, Move> AlphaBetaSearch(int alpha, int beta, uint_fast8_t depth_left) noexcept;
    std::pair<int, Move> NegamaxAlphaBetaSearch(int alpha, int beta, uint_fast8_t depth_left) noexcept;
    int Quiesce(int alpha, int beta, bool initial_turn) noexcept;
private:
    Board board;
    uint_fast8_t depth = 2;
    PieceValue piece_value;
    Move default_move{9, 9, 9, 9, 9, 9};
    Zobrist_Hash zobrist_hash;

    std::chrono::duration<double, std::milli> last_eval_time;

    const int INF = INT_MAX;
};

#endif // ENGINE_H
