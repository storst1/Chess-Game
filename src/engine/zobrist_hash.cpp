#include "zobrist_hash.h"

//Creates uninitialized object that is NOT ready to be used properly
Zobrist_Hash::Zobrist_Hash()
{

}

//Initializes zobrish hash object with random values
void Zobrist_Hash::Init() noexcept
{
    for(int i = 0; i < 12; ++i){
        for(int j = 0; j < 8; ++j){
            for(int g = 0; g < 8; ++g){
                pieces_hash[i][j][g] = QRandomGenerator::global()->generate64();
            }
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

uint_fast64_t &Zobrist_Hash::Piece(int_fast8_t piece, uint_fast8_t x, uint_fast8_t y) noexcept
{
    if(piece < 0){
        piece += 13;
    }
    return pieces_hash[piece - 1][x][y];
}

uint_fast64_t &Zobrist_Hash::Move() noexcept
{
    return black_move;
}

uint_fast64_t &Zobrist_Hash::WCK() noexcept
{
    return white_castle_kingside;
}

uint_fast64_t &Zobrist_Hash::WCQ() noexcept
{
    return white_castle_queenside;
}

uint_fast64_t &Zobrist_Hash::BCK() noexcept
{
    return black_castle_kingside;
}

uint_fast64_t &Zobrist_Hash::BCQ() noexcept
{
    return black_castle_queenside;
}
