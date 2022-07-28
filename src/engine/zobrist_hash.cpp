#include "zobrist_hash.h"

//Creates uninitialized object that is not ready to be used properly
Zobrist_Hash::Zobrist_Hash()
{

}

//Initializes zobrish hash object with random values
void Zobrist_Hash::Init() noexcept
{
    for(int i = 0; i < 12; ++i){
        for(int j = 0; j < 64; ++j){
            pieces_hash[i][j] = QRandomGenerator::global()->generate64();
        }
    }
    for(int i = 0; i < 8; ++i){
        en_passant_files[i] = QRandomGenerator::global()->generate64();
    }
    black_move = QRandomGenerator::global()->generate64();
    white_castle_kingside = QRandomGenerator::global()->generate64();
    white_castle_queenside = QRandomGenerator::global()->generate64();
    black_castle_kingside = QRandomGenerator::global()->generate64();
    black_castle_queenside = QRandomGenerator::global()->generate64();
}

/*
constexpr uint_fast64_t Zobrist_Hash::UpperBound() noexcept
{
    return UINT64_MAX;
}

constexpr uint_fast64_t Zobrist_Hash::LowerBound() noexcept
{
    return 100;
}
*/
