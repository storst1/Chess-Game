#ifndef POSSIBLEMOVESVECTOR_H
#define POSSIBLEMOVESVECTOR_H

#include "move.h"
#include "direction.h"

class PossibleMovesVector{
public:
    PossibleMovesVector(Board& board);
    PossibleMovesVector(const PossibleMovesVector& other) noexcept;
    ~PossibleMovesVector() = default;

    std::vector<Move>::const_iterator begin() const;
    std::vector<Move>::const_iterator end() const;
    std::vector<Move>::iterator begin();
    std::vector<Move>::iterator end();

    void push_back(Move move);
    void push_back(Move& move);
    void reserve(size_t size);
    void clear();
    size_t size() const noexcept;
    bool empty() const noexcept;
private:
    std::vector<Move> moves;
    Board& board;
};
#endif // POSSIBLEMOVESVECTOR_H
