#ifndef PIECEVALUE_H
#define PIECEVALUE_H

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

struct PieceValue{
    PieceValue();
    ~PieceValue() = default;

    int GetPieceValue(int_fast8_t piece);

    unsigned int pawn_cost;
    unsigned int knight_cost;
    unsigned int bishop_cost;
    unsigned int rook_cost;
    unsigned int queen_cost;
    unsigned int king_cost;
};

#endif // PIECEVALUE_H
