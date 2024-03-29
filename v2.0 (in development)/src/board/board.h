#ifndef BOARD_H
#define BOARD_H

#include <cstdint>
#include <algorithm>
#include <stdexcept>
#include <vector>
#include <cmath>

#include <QVector>
#include <QLabel>
#include <QPixmap>
#include <QPainter>

#include "move.h"
#include "direction.h"
#include "coords.h"
#include "possible_moves_vector.h"
#include "engine/piece_value.h"

class Engine;

class Board
{
public:
    Board() noexcept;
    Board(const Board& parent) noexcept;
    ~Board() = default;

    bool LoadFromFEN(const QString &FEN);
    bool Turn() const noexcept;
    int_fast8_t TotalTurns() const noexcept;
    int_fast8_t& Get(uint_fast8_t x, uint_fast8_t y) noexcept;
    int_fast8_t At(uint_fast8_t x, uint_fast8_t y) const;
    Move& LastMove();
    Coords KingPos(bool color) const noexcept;
    Coords FindKing(bool color) const noexcept;
    bool IsDefended(uint_fast8_t x, uint_fast8_t y) const noexcept;
    void CalculateAllMoves() noexcept;
    bool CheckIfSquareDefended(uint_fast8_t x, uint_fast8_t y) const noexcept;
    bool CheckIfMoveIsAllowed(uint_fast8_t x, uint_fast8_t y, DIR& dir) const noexcept;
    bool IsCheck() const noexcept;
    uint_fast8_t ChecksAmount() const noexcept;
    bool IsCheckmate() const noexcept;
    bool WCK_Possible() const noexcept; //White castle kingside possible
    bool WCQ_Possible() const noexcept; //White castle queenside possible
    bool BCK_Possible() const noexcept; //Black castle kingside possible
    bool BCQ_Possible() const noexcept; //Black castle queenside possible
    void ClearEverything() noexcept;
    void ClearBoardPieces() noexcept;
    void SetDefaultStartingBoard() noexcept;
    size_t PossibleMovesStartingCapacity() const noexcept;
    size_t MovesPlayedStartingCapacity() const noexcept;
    int TotalBishops() const noexcept;
    int TotalPawns() const noexcept;
    int TotalKnights() const noexcept;
    int TotalRooks() const noexcept;
    int TotalQueens() const noexcept;
    int TotalKings() const noexcept;
    int TotalEmptySquares() const noexcept;
    int TotalBishops(bool color) const noexcept;
    int TotalPawns(bool color) const noexcept;
    int TotalKnights(bool color) const noexcept;
    int TotalRooks(bool color) const noexcept;
    int TotalQueens(bool color) const noexcept;
    int TotalKings(bool color) const noexcept;
    int TotalPieces() const noexcept;
    int TotalPieces(bool color) const noexcept;

    static bool OnBoard(uint_fast8_t x, uint_fast8_t y) noexcept;
    static QPixmap GetPiecePixmap(int_fast8_t type) noexcept;
    static QChar FileToChar(uint_fast8_t file) noexcept;

    //API for window class to interact with a user
    void DrawBoard(QPainter* painter, int cx = -1, int cy = -1) const noexcept;
    bool CheckIfPieceWakable(int x, int y) const noexcept;
    bool IsLegalMove(int x1, int y1, int x2, int y2) const noexcept;
    void ApplyUserMove(int x1, int y1, int x2, int y2);

    //API for engine
    PossibleMovesVector& PossibleMovesRef() noexcept;
    PossibleMovesVector PossibleMoves() const noexcept;
    void RunMove(const Move &move);
    void RevertLastMove() noexcept;
    std::vector<Move> PlayedMoves() const noexcept;
    std::vector<Move>& PlayedMovesRef() noexcept;
    std::pair<DIR, Coords>& GetCheckByIdx(uint_fast8_t idx) noexcept;

    //Debug functions
    void D_PrintDefendedSquares() const noexcept;
    void D_PrintBoard() const noexcept;
private:
    void CalculateMoves(uint_fast8_t x, uint_fast8_t y) noexcept;
    void CalculatePawnMoves(uint_fast8_t x, uint_fast8_t y) noexcept;
    void CalculateKnightMoves(uint_fast8_t x, uint_fast8_t y) noexcept;
    void CalculateBishopMoves(uint_fast8_t x, uint_fast8_t y) noexcept;
    void CalculateRookMoves(uint_fast8_t x, uint_fast8_t y) noexcept;
    void CalculateQueenMoves(uint_fast8_t x, uint_fast8_t y) noexcept;
    void CalculateKingMoves(uint_fast8_t x, uint_fast8_t y) noexcept;
    void CalculateSlidingMoves(const uint_fast8_t x, const uint_fast8_t y,
                               int_fast8_t x_del, int_fast8_t y_del) noexcept;
    void CalculateAdditionalInfo() noexcept;
    void CalculateAdditionalInfoForPiece(uint_fast8_t x, uint_fast8_t y) noexcept;
    void CalculateInfoPawn(uint_fast8_t x, uint_fast8_t y) noexcept;
    void CalculateInfoKnight(uint_fast8_t x, uint_fast8_t y) noexcept;
    void CalculateInfoBishop(uint_fast8_t x, uint_fast8_t y) noexcept;
    void CalculateInfoRook(uint_fast8_t x, uint_fast8_t y) noexcept;
    void CalculateInfoQueen(uint_fast8_t x, uint_fast8_t y) noexcept;
    void CalculateInfoKing(uint_fast8_t x, uint_fast8_t y) noexcept;
    void CalculateInfoSlidingTowards(const uint_fast8_t x, const uint_fast8_t y,
                                    int_fast8_t x_del, int_fast8_t y_del,
                                    const Direction::Dir dir, Direction::Dir opp_dir = Direction::NONE) noexcept;

    bool Opposite(bool turn, int_fast8_t square) const noexcept;
    bool Same(bool turn, int_fast8_t square) const noexcept;
    void ClearAllowedMoves() noexcept;
    void ClearDefendedSquares() noexcept;
    void SetAllCastleVars(bool value) noexcept;
private:
    int_fast8_t board[8][8] =
    {
        {-4, -1, -0, -0, +0, +0, +1, +4},
        {-2, -1, -0, -0, +0, +0, +1, +2},
        {-3, -1, -0, -0, +0, +0, +1, +3},
        {-5, -1, -0, -0, +0, +0, +1, +5},
        {-6, -1, -0, -0, +0, +0, +1, +6},
        {-3, -1, -0, -0, +0, +0, +1, +3},
        {-2, -1, -0, -0, +0, +0, +1, +2},
        {-4, -1, -0, -0, +0, +0, +1, +4}
    };
    bool turn = true;
    uint_fast16_t total_turns = 0;
    PossibleMovesVector possible_moves;
    bool defended_squares[8][8];
    std::vector<std::vector<std::vector<DIR>>> allowed_moves;
    std::vector<Move> moves_played;
    bool black_king_moved = false;
    bool white_king_moved = false;
    bool black_kingside_rook_moved = false;
    bool black_queenside_rook_moved = false;
    bool white_kingside_rook_moved = false;
    bool white_queenside_rook_moved = false;
    std::vector<std::pair<DIR, Coords>> checks;
    Coords black_king_coors{4, 0};
    Coords white_king_coords{4, 7};

    //Constant fields
    const QColor black_square = QColor(209, 139, 71);
    const QColor white_square = QColor(255, 206, 158);
    const QColor highlighted_square = QColor(1, 241, 254);
    const QColor current_square = QColor(255, 252, 128);
    const QColor check_on_king_square = QColor(255, 0, 0);
    const int sp = 100; //Pixels per square (tile)
    const size_t possible_moves_capacity = 60;
    const size_t moves_played_capacity = 200;
};

#endif // BOARD_H
