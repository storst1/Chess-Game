#ifndef ZOBRIST_HASH_H
#define ZOBRIST_HASH_H

#include <cstdint>

#include <QRandomGenerator>
#include <QString>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QTextStream>
#include <QByteArray>
#include <QDir>
#include <QJsonValue>

class Board;

class Zobrist_Hash
{
public:
    Zobrist_Hash();
    ~Zobrist_Hash() = default;

    void Init() noexcept;
    bool IsInitValidly() const noexcept;
    uint_fast64_t CalculateHashValue(Board& board) noexcept;

    uint_fast64_t& PieceHash(int_fast8_t piece, uint_fast8_t x, uint_fast8_t y) noexcept;
    uint_fast64_t& Move() noexcept;
    uint_fast64_t& WCK() noexcept;
    uint_fast64_t& WCQ() noexcept;
    uint_fast64_t& BCK() noexcept;
    uint_fast64_t& BCQ() noexcept;

    void SaveAsJsonFile(QString path) const;
    void LoadFromJsonFile(QString path);

    static QString DefaultZHPath() noexcept;
private:
    bool initialized_correctly = false;
    uint_fast64_t pieces_hash[12][8][8];
    uint_fast64_t black_move;
    uint_fast64_t en_passant_files[8];
    uint_fast64_t white_castle_kingside;
    uint_fast64_t white_castle_queenside;
    uint_fast64_t black_castle_kingside;
    uint_fast64_t black_castle_queenside;
};

#endif // ZOBRIST_HASH_H
