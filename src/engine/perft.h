#ifndef PERFT_H
#define PERFT_H

/*
 * This file contains struct to save data that was collected durind Engine::Perft() search.
 * Perft search is used only for debbuging purposes to make sure that move generator works
 * precisely and without any bugs
*/

#include <cstdint>

#include <QString>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

struct PerftInfo{
    PerftInfo(const uint_fast8_t depth);
    ~PerftInfo() = default;

    void ResetData() noexcept;
    bool LoadDataFromJson(const QString& path);
    std::pair<bool, QString> operator ==(const PerftInfo& other) const noexcept;
    std::pair<bool, QString> operator !=(const PerftInfo& other) const noexcept;

    const uint_fast8_t search_depth;
    uint_fast64_t nodes = 0;
    uint_fast64_t captures = 0;
    uint_fast64_t en_passant_moves = 0;
    uint_fast64_t castle_moves = 0;
    uint_fast64_t promotions = 0;
    uint_fast64_t checks = 0;
    uint_fast64_t checkmates = 0;
};

#endif // PERFT_H
