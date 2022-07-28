#ifndef ZOBRIST_HASH_H
#define ZOBRIST_HASH_H

#include <cstdint>

#include <QRandomGenerator>

class Zobrist_Hash
{
public:
    Zobrist_Hash();
    ~Zobrist_Hash() = default;

    void Init() noexcept;

    //static constexpr uint_fast64_t UpperBound() noexcept;
    //static constexpr uint_fast64_t LowerBound() noexcept;
private:
    uint_fast64_t pieces_hash[12][64];
    uint_fast64_t black_move;
    uint_fast64_t en_passant_files[8];
    uint_fast64_t white_castle_kingside;
    uint_fast64_t white_castle_queenside;
    uint_fast64_t black_castle_kingside;
    uint_fast64_t black_castle_queenside;
};

#endif // ZOBRIST_HASH_H
