#ifndef PIECEVALUE_H
#define PIECEVALUE_H

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

struct PieceValue{
    PieceValue(const QString &path);
    ~PieceValue() = default;

    int GetPieceValue(int_fast8_t piece);
    void D_PrintPieceValues() const noexcept;

    unsigned int pawn_cost;
    unsigned int knight_cost;
    unsigned int bishop_cost;
    unsigned int rook_cost;
    unsigned int queen_cost;
    unsigned int king_cost;

    template<typename T>
    static int Sgn(T val) {
        return (T(0) < val) - (val < T(0));
    }
};

#endif // PIECEVALUE_H
