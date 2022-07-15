#include "board.h"

Board::Board() noexcept : possible_moves(*this)
{
    possible_moves.reserve(50);
    moves_played.reserve(150);
    allowed_moves.resize(8, std::vector<std::vector<DIR>> (8, std::vector<DIR>(0)));
    Move m(6, 4, 7, 2, 7, 0, 8, 8, true);
}

Board::Board(const Board &parent) noexcept
    : turn(parent.turn),
      total_turns(parent.total_turns),
      possible_moves(parent.possible_moves),
      moves_played(parent.moves_played)
{
    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            board[i][j] = parent.board[i][j];
        }
    }
}

void Board::DrawBoard(QPainter *painter, int cx, int cy) const noexcept
{
    bool painted[8][8]{};

    if(IsCheck()){
        Coords king_pos = FindKing(turn);
        painter->fillRect(king_pos.x * sp + 1, king_pos.y * sp + 1, sp - 1, sp - 1, check_on_king_square);
        painted[king_pos.x][king_pos.y] = true;
    }

    if(cx != -1){
        for(const auto& move : possible_moves){
            if(move.x1 == cx && move.y1 == cy){
               painter->fillRect(move.x1 * sp + 1, move.y1 * sp + 1, sp - 1, sp - 1, current_square);
               painter->fillRect(move.x2 * sp + 1, move.y2 * sp + 1, sp - 1, sp - 1, highlighted_square);
               painted[move.x1][move.y1] = true;
               painted[move.x2][move.y2] = true;
            }
        }
    }

    bool color = true;
    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            if(!painted[i][j]){
                painter->fillRect(i * sp, j * sp, sp, sp, (color ? white_square : black_square));
            }
            if(board[i][j]){
                //qDebug() << "Drawn piece on " << i << " " << j << "\n";
                painter->drawPixmap(i * sp, j * sp, GetPiecePixmap(board[i][j]).scaled(100, 100, Qt::IgnoreAspectRatio));
            }
            color = !color;
        }
        color = !color;
    }
}

bool Board::Turn() const noexcept
{
    return turn;
}

int_fast8_t Board::TotalTurns() const noexcept
{
    return total_turns;
}

//Does not throw exception if (x, y) are out of the board and returns referense to a square rather then its value
int_fast8_t &Board::Get(uint_fast8_t x, uint_fast8_t y) noexcept
{
    return board[x][y];
}

//Throws exception if (x, y) are out of the board, otherwise returns value of the given square
int_fast8_t Board::At(uint_fast8_t x, uint_fast8_t y) const
{
    if(x > 7 || y > 7){
        throw std::out_of_range("Provided arguments for the Board::At() fucntion are out of the logical board border!");
    }
    return board[x][y];
}

Move &Board::LastMove()
{
    if(moves_played.empty()){
        throw std::out_of_range("Cannot get a last move in Board::LastMove() function since no moves was played");
    }
    return moves_played[moves_played.size() - 1];
}

Coords Board::FindKing(bool color) const noexcept
{
    return (color ? white_king_coords : black_king_coors);
}

//Calculates all the possible moves on the board for the color that ows the current turn
void Board::CalculatePossibleMoves() noexcept
{
    checks.clear();
    possible_moves.clear();
    ClearAllowedMoves();
    ClearDefendedSquares();
    CalculateAdditionalInfo();

    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            if(turn && board[i][j] > 0){ //If white owns current move
                CalculateMoves(i, j);
            }
            else if(!turn && board[i][j] < 0){ //If black owns current move
                CalculateMoves(i, j);
            }
            //Else means square is empty(==0) and does not have a piece on it
        }
    }
}

bool Board::OnBoard(uint_fast8_t x, uint_fast8_t y) noexcept
{
    if(x > 7 || y > 7){
        return false;
    }
    return true;
}

//Determines what moves calculation function to call based on (x, y) coords piece
void Board::CalculateMoves(uint_fast8_t x, uint_fast8_t y) noexcept
{
    if(abs(board[x][y]) == 1){
        CalculatePawnMoves(x, y);
    }
    else if(abs(board[x][y]) == 2){
        CalculateKnightMoves(x, y);
    }
    else if(abs(board[x][y]) == 3){
        CalculateBishopMoves(x, y);
    }
    else if(abs(board[x][y]) == 4){
        CalculateRookMoves(x, y);
    }
    else if(abs(board[x][y]) == 5){
        CalculateQueenMoves(x, y);
    }
    else if(abs(board[x][y]) == 6){
        CalculateKingMoves(x, y);
    }
}

void Board::CalculatePawnMoves(uint_fast8_t x, uint_fast8_t y) noexcept
{
    //Check if pawn can move 2 squares forward
    if(!turn && y == 1 && !board[x][y + 2] && !board[x][y + 1]){
        possible_moves.push_back(Move(board[x][y], x, y, x, y + 2, board[x][y + 2]));
    }
    else if(turn && y == 6 && !board[x][y - 2] && !board[x][y - 1]){
        possible_moves.push_back(Move(board[x][y], x, y, x, y - 2, board[x][y - 2]));
    }

    //Check if pawn can make a regular move forward
    if(!turn && !board[x][y + 1]){
        possible_moves.push_back(Move(board[x][y], x, y, x, y + 1, board[x][y + 1]));
    }
    else if(turn && !board[x][y - 1]){
        possible_moves.push_back(Move(board[x][y], x, y, x, y - 1, board[x][y - 1]));
    }

    //Check if capture possible
    if(!turn){
        if(OnBoard(x + 1, y + 1) && (board[x + 1][y + 1] > 0)){
            possible_moves.push_back(Move(board[x][y], x, y, x + 1, y + 1, board[x + 1][y + 1]));
        }
        if(OnBoard(x - 1, y + 1) && (board[x - 1][y + 1] > 0)){
            possible_moves.push_back(Move(board[x][y], x, y, x - 1, y + 1, board[x - 1][y + 1]));
        }
    }
    else{
        if(OnBoard(x + 1, y - 1) && (board[x + 1][y - 1] < 0)){
            possible_moves.push_back(Move(board[x][y], x, y, x + 1, y - 1, board[x + 1][y - 1]));
        }
        if(OnBoard(x - 1, y - 1) && (board[x - 1][y - 1] < 0)){
            possible_moves.push_back(Move(board[x][y], x, y, x - 1, y - 1, board[x - 1][y - 1]));
        }
    }

    //Check if en passant move is possible
    if(moves_played.empty()){
        return;
    }
    Move last_move = LastMove();
    if(abs(last_move.piece) == 1 && abs(last_move.y1 - last_move.y2) == 2){
        //Means that last enemy move was 2 squares forward pawn move
        if(turn){
            if(last_move.y2 - 1 == y - 1 && last_move.x1 == x - 1){
                possible_moves.push_back(Move(board[x][y], x, y, x - 1, y - 1, board[x - 1][y - 1], last_move.x2, last_move.y2));
            }
            if(last_move.y2 - 1 == y - 1 && last_move.x1 == x + 1){
                possible_moves.push_back(Move(board[x][y], x, y, x + 1, y - 1, board[x + 1][y - 1], last_move.x2, last_move.y2));
            }
        }
        else{
            if(last_move.y2 + 1 == y + 1 && last_move.x1 == x - 1){
                possible_moves.push_back(Move(board[x][y], x, y, x - 1, y + 1, board[x - 1][y + 1], last_move.x2, last_move.y2));
            }
            if(last_move.y2 + 1 == y + 1 && last_move.x1 == x + 1){
                possible_moves.push_back(Move(board[x][y], x, y, x + 1, y + 1, board[x + 1][y + 1], last_move.x2, last_move.y2));
            }
        }
    }
}

void Board::CalculateKnightMoves(uint_fast8_t x, uint_fast8_t y) noexcept
{
    if(OnBoard(x + 1, y + 2) && (Opposite(turn, board[x + 1][y + 2]) || !board[x + 1][y + 2])){
        possible_moves.push_back(Move(board[x][y], x, y, x + 1, y + 2, board[x + 1][y + 2]));
    }
    if(OnBoard(x + 1, y - 2) && (Opposite(turn, board[x + 1][y - 2]) || !board[x + 1][y - 2])){
        possible_moves.push_back(Move(board[x][y], x, y, x + 1, y - 2, board[x + 1][y - 2]));
    }
    if(OnBoard(x + 2, y + 1) && (Opposite(turn, board[x + 2][y + 1]) || !board[x + 2][y + 1])){
        possible_moves.push_back(Move(board[x][y], x, y, x + 2, y + 1, board[x + 2][y + 1]));
    }
    if(OnBoard(x + 2, y - 1) && (Opposite(turn, board[x + 2][y - 1]) || !board[x + 2][y - 1])){
        possible_moves.push_back(Move(board[x][y], x, y, x + 2, y - 1, board[x + 2][y - 1]));
    }
    if(OnBoard(x - 2, y + 1) && (Opposite(turn, board[x - 2][y + 1]) || !board[x - 2][y + 1])){
        possible_moves.push_back(Move(board[x][y], x, y, x - 2, y + 1, board[x - 2][y + 1]));
    }
    if(OnBoard(x - 2, y - 1) && (Opposite(turn, board[x - 2][y - 1]) || !board[x - 2][y - 1])){
        possible_moves.push_back(Move(board[x][y], x, y, x - 2, y - 1, board[x - 2][y - 1]));
    }
    if(OnBoard(x - 1, y + 2) && (Opposite(turn, board[x - 1][y + 2]) || !board[x - 1][y + 2])){
        possible_moves.push_back(Move(board[x][y], x, y, x - 1, y + 2, board[x - 1][y + 2]));
    }
    if(OnBoard(x - 1, y - 2) && (Opposite(turn, board[x - 1][y - 2]) || !board[x - 1][y - 2])){
        possible_moves.push_back(Move(board[x][y], x, y, x - 1, y - 2, board[x - 1][y - 2]));
    }
}

void Board::CalculateBishopMoves(uint_fast8_t x, uint_fast8_t y) noexcept
{
    uint_fast8_t nx = x - 1, ny = y - 1;
    while(OnBoard(nx, ny)){
        if(board[nx][ny] == 0){
            possible_moves.push_back(Move(board[x][y], x, y, nx, ny, board[nx][ny]));
        }
        else if(Same(turn, board[nx][ny])){
            break;
        }
        else if(Opposite(turn, board[nx][ny])){
            possible_moves.push_back(Move(board[x][y], x, y, nx, ny, board[nx][ny]));
            break;
        }
        --nx; --ny;
    }

    nx = x + 1, ny = y - 1;
    while(OnBoard(nx, ny)){
        if(board[nx][ny] == 0){
            possible_moves.push_back(Move(board[x][y], x, y, nx, ny, board[nx][ny]));
        }
        else if(Same(turn, board[nx][ny])){
            break;
        }
        else if(Opposite(turn, board[nx][ny])){
            possible_moves.push_back(Move(board[x][y], x, y, nx, ny, board[nx][ny]));
            break;
        }
        ++nx; --ny;
    }

    nx = x + 1, ny = y + 1;
    while(OnBoard(nx, ny)){
        if(board[nx][ny] == 0){
            possible_moves.push_back(Move(board[x][y], x, y, nx, ny, board[nx][ny]));
        }
        else if(Same(turn, board[nx][ny])){
            break;
        }
        else if(Opposite(turn, board[nx][ny])){
            possible_moves.push_back(Move(board[x][y], x, y, nx, ny, board[nx][ny]));
            break;
        }
        ++nx; ++ny;
    }

    nx = x - 1, ny = y + 1;
    while(OnBoard(nx, ny)){
        if(board[nx][ny] == 0){
            possible_moves.push_back(Move(board[x][y], x, y, nx, ny, board[nx][ny]));
        }
        else if(Same(turn, board[nx][ny])){
            break;
        }
        else if(Opposite(turn, board[nx][ny])){
            possible_moves.push_back(Move(board[x][y], x, y, nx, ny, board[nx][ny]));
            break;
        }
        --nx; ++ny;
    }
}

void Board::CalculateRookMoves(uint_fast8_t x, uint_fast8_t y) noexcept
{
    uint_fast8_t nx = x - 1, ny = y;
    while(OnBoard(nx, ny)){
        if(board[nx][ny] == 0){
            possible_moves.push_back(Move(board[x][y], x, y, nx, ny, board[nx][ny]));
        }
        else if(Same(turn, board[nx][ny])){
            break;
        }
        else if(Opposite(turn, board[nx][ny])){
            possible_moves.push_back(Move(board[x][y], x, y, nx, ny, board[nx][ny]));
            break;
        }
        --nx;
    }

    nx = x + 1;
    while(OnBoard(nx, ny)){
        if(board[nx][ny] == 0){
            possible_moves.push_back(Move(board[x][y], x, y, nx, ny, board[nx][ny]));
        }
        else if(Same(turn, board[nx][ny])){
            break;
        }
        else if(Opposite(turn, board[nx][ny])){
            possible_moves.push_back(Move(board[x][y], x, y, nx, ny, board[nx][ny]));
            break;
        }
        ++nx;
    }

    nx = x; ny = y - 1;
    while(OnBoard(nx, ny)){
        if(board[nx][ny] == 0){
            possible_moves.push_back(Move(board[x][y], x, y, nx, ny, board[nx][ny]));
        }
        else if(Same(turn, board[nx][ny])){
            break;
        }
        else if(Opposite(turn, board[nx][ny])){
            possible_moves.push_back(Move(board[x][y], x, y, nx, ny, board[nx][ny]));
            break;
        }
        --ny;
    }

    ny = y + 1;
    while(OnBoard(nx, ny)){
        if(board[nx][ny] == 0){
            possible_moves.push_back(Move(board[x][y], x, y, nx, ny, board[nx][ny]));
        }
        else if(Same(turn, board[nx][ny])){
            break;
        }
        else if(Opposite(turn, board[nx][ny])){
            possible_moves.push_back(Move(board[x][y], x, y, nx, ny, board[nx][ny]));
            break;
        }
        ++ny;
    }
}

void Board::CalculateQueenMoves(uint_fast8_t x, uint_fast8_t y) noexcept
{
    CalculateBishopMoves(x, y);
    CalculateRookMoves(x, y);
}

void Board::CalculateKingMoves(uint_fast8_t x, uint_fast8_t y) noexcept
{
    if(OnBoard(x - 1, y - 1) && (!board[x - 1][y - 1] || (Opposite(turn, board[x - 1][y - 1]) && !defended_squares[x - 1][y - 1]))){
        possible_moves.push_back(Move(board[x][y], x, y, x - 1, y - 1, board[x - 1][y - 1]));
    }
    if(OnBoard(x - 1, y + 1) && (!board[x - 1][y + 1] || (Opposite(turn, board[x - 1][y + 1]) && !defended_squares[x - 1][y + 1]))){
        possible_moves.push_back(Move(board[x][y], x, y, x - 1, y + 1, board[x - 1][y + 1]));
    }
    if(OnBoard(x + 1, y + 1) && (!board[x + 1][y + 1] || (Opposite(turn, board[x + 1][y + 1]) && !defended_squares[x + 1][y + 1]))){
        possible_moves.push_back(Move(board[x][y], x, y, x + 1, y + 1, board[x + 1][y + 1]));
    }
    if(OnBoard(x + 1, y - 1) && (!board[x + 1][y - 1] || (Opposite(turn, board[x + 1][y - 1]) && !defended_squares[x + 1][y - 1]))){
        possible_moves.push_back(Move(board[x][y], x, y, x + 1, y - 1, board[x + 1][y - 1]));
    }
    if(OnBoard(x, y + 1) && (!board[x][y + 1] || (Opposite(turn, board[x][y + 1]) && !defended_squares[x][y + 1]))){
        possible_moves.push_back(Move(board[x][y], x, y, x, y + 1, board[x][y + 1]));
    }
    if(OnBoard(x, y - 1) && (!board[x][y - 1] || (Opposite(turn, board[x][y - 1]) && !defended_squares[x][y - 1]))){
        possible_moves.push_back(Move(board[x][y], x, y, x, y - 1, board[x][y - 1]));
    }
    if(OnBoard(x + 1, y) && (!board[x + 1][y] || (Opposite(turn, board[x + 1][y]) && !defended_squares[x + 1][y]))){
        possible_moves.push_back(Move(board[x][y], x, y, x + 1, y, board[x + 1][y]));
    }
    if(OnBoard(x - 1, y) && (!board[x - 1][y] || (Opposite(turn, board[x - 1][y]) && !defended_squares[x - 1][y]))){
        possible_moves.push_back(Move(board[x][y], x, y, x - 1, y, board[x - 1][y]));
    }

    //Check if castle possible
    if(turn){
        if(white_king_moved){
            return;
        }
        if(!white_queenside_rook_moved){
            bool castle_available = true;
            for(uint_fast8_t i = 1; i < 4; ++i){
                if(board[i][7] || defended_squares[i][7]){
                    castle_available = false;
                    break;
                }
            }
            if(castle_available){
                possible_moves.push_back(Move(6, 4, 7, 2, 7, 0, 8, 8, true));
            }
        }
        if(!white_kingside_rook_moved){
            bool castle_available = true;
            for(uint_fast8_t i = 5; i < 7; ++i){
                if(board[i][7] || defended_squares[i][7]){
                    castle_available = false;
                    break;
                }
            }
            if(castle_available){
                possible_moves.push_back(Move(6, 4, 7, 6, 7, 0, 8, 8, true));
            }
        }
    }
    else{
        if(black_king_moved){
            return;
        }
        if(!black_queenside_rook_moved){
            bool castle_available = true;
            for(uint_fast8_t i = 1; i < 4; ++i){
                if(board[i][0] || defended_squares[i][0]){
                    castle_available = false;
                    break;
                }
            }
            if(castle_available){
                possible_moves.push_back(Move(-6, 4, 0, 2, 0, 0, 8, 8, true));
            }
        }
        if(!black_kingside_rook_moved){
            bool castle_available = true;
            for(uint_fast8_t i = 5; i < 7; ++i){
                if(board[i][0] || defended_squares[i][0]){
                    castle_available = false;
                    break;
                }
            }
            if(castle_available){
                possible_moves.push_back(Move(-6, 4, 0, 6, 0, 0, 8, 8, true));
            }
        }
    }
}

void Board::CalculateAdditionalInfo() noexcept
{
    for(uint_fast8_t i = 0; i < 8; ++i){
        for(uint_fast8_t j = 0; j < 8; ++j){
            if(Opposite(turn, board[i][j])){
                CalculateAdditionalInfoForPiece(i, j);
            }
        }
    }
}

void Board::CalculateAdditionalInfoForPiece(uint_fast8_t x, uint_fast8_t y) noexcept
{
    if(abs(board[x][y]) == 1){
        CalculateInfoPawn(x, y);
    }
    else if(abs(board[x][y]) == 2){
        CalculateInfoKnight(x, y);
    }
    else if(abs(board[x][y]) == 3){
        CalculateInfoBishop(x, y);
    }
    else if(abs(board[x][y]) == 4){
        CalculateInfoRook(x, y);
    }
    else if(abs(board[x][y]) == 5){
        CalculateInfoQueen(x, y);
    }
    else if(abs(board[x][y]) == 6){
        CalculateInfoKing(x, y);
    }
}

void Board::CalculateInfoPawn(uint_fast8_t x, uint_fast8_t y) noexcept
{
    if(turn){
        if(OnBoard(x + 1, y + 1)){
            defended_squares[x + 1][y + 1] = true;
            if(board[x + 1][y + 1] == 6){
                checks.push_back({DIR::DR, Coords{x, y}});
            }
        }
        if(OnBoard(x - 1, y + 1)){
            defended_squares[x - 1][y + 1] = true;
            if(board[x - 1][y - 1] == 6){
                checks.push_back({DIR::DL, Coords{x, y}});
            }
        }
    }
    else{
        if(OnBoard(x + 1, y - 1)){
            defended_squares[x + 1][y - 1] = true;
            if(board[x + 1][y - 1] == -6){
                checks.push_back({DIR::UR, Coords{x, y}});
            }
        }
        if(OnBoard(x - 1, y - 1)){
            defended_squares[x - 1][y - 1] = true;
            if(board[x - 1][y - 1] == -6){
                checks.push_back({DIR::UL, Coords{x, y}});
            }
        }
    }
}

void Board::CalculateInfoKnight(uint_fast8_t x, uint_fast8_t y) noexcept
{
    if(OnBoard(x + 1, y + 2)){
        defended_squares[x + 1][y + 2] = true;
        if(board[x + 1][y + 2] == (turn ? 6 : -6)){
            checks.push_back({DIR::KM, Coords{x, y}});
        }
    }
    if(OnBoard(x + 1, y - 2)){
        defended_squares[x + 1][y - 2] = true;
        if(board[x + 1][y - 2] == (turn ? 6 : -6)){
            checks.push_back({DIR::KM, Coords{x, y}});
        }
    }
    if(OnBoard(x + 2, y + 1)){
        defended_squares[x + 2][y + 1] = true;
        if(board[x + 2][y + 1] == (turn ? 6 : -6)){
            checks.push_back({DIR::KM, Coords{x, y}});
        }
    }
    if(OnBoard(x + 2, y - 1)){
        defended_squares[x + 2][y - 1] = true;
        if(board[x + 2][y - 1] == (turn ? 6 : -6)){
            checks.push_back({DIR::KM, Coords{x, y}});
        }
    }
    if(OnBoard(x - 2, y + 1)){
        defended_squares[x - 2][y + 1] = true;
        if(board[x - 2][y + 1] == (turn ? 6 : -6)){
            checks.push_back({DIR::KM, Coords{x, y}});
        }
    }
    if(OnBoard(x - 2, y - 1)){
        defended_squares[x - 2][y - 1] = true;
        if(board[x - 2][y - 1] == (turn ? 6 : -6)){
            checks.push_back({DIR::KM, Coords{x, y}});
        }
    }
    if(OnBoard(x - 1, y + 2)){
        defended_squares[x - 1][y + 2] = true;
        if(board[x - 1][y + 2] == (turn ? 6 : -6)){
            checks.push_back({DIR::KM, Coords{x, y}});
        }
    }
    if(OnBoard(x - 1, y - 2)){
        defended_squares[x - 1][y - 2] = true;
        if(board[x - 1][y - 2] == (turn ? 6 : -6)){
            checks.push_back({DIR::KM, Coords{x, y}});
        }
    }
}

void Board::CalculateInfoBishop(uint_fast8_t x, uint_fast8_t y) noexcept
{
    uint_fast8_t nx = x - 1, ny = y - 1;
    bool met_opposite = false;
    Coords first_opposite;
    while(OnBoard(nx, ny)){
        if(board[nx][ny] == 0){
            defended_squares[nx][ny] = true;
        }
        else if(Opposite(turn, board[nx][ny])){
            defended_squares[nx][ny] = true;
            break;
        }
        else if(Same(turn, board[nx][ny])){
            if(!met_opposite){
                met_opposite = true;
                first_opposite = {nx, ny};
                if(board[nx][ny] == (turn ? 6 : -6)){
                    checks.push_back({DIR::UL, Coords{x, y}});
                    break;
                }
            }
            else{
                if(board[nx][ny] == (turn ? 6 : -6)){
                    allowed_moves[first_opposite.x][first_opposite.y].push_back(DIR::DR);
                }
                break;
            }
        }
        --nx; --ny;
    }

    nx = x + 1, ny = y - 1;
    met_opposite = false;
    while(OnBoard(nx, ny)){
        if(board[nx][ny] == 0){
            defended_squares[nx][ny] = true;
        }
        else if(Opposite(turn, board[nx][ny])){
            defended_squares[nx][ny] = true;
            break;
        }
        else if(Same(turn, board[nx][ny])){
            if(!met_opposite){
                met_opposite = true;
                first_opposite = {nx, ny};
                if(board[nx][ny] == (turn ? 6 : -6)){
                    checks.push_back({DIR::UR, Coords{x, y}});
                    break;
                }
            }
            else{
                if(board[nx][ny] == (turn ? 6 : -6)){
                    allowed_moves[first_opposite.x][first_opposite.y].push_back(DIR::DL);
                }
                break;
            }
        }
        ++nx; --ny;
    }

    nx = x + 1, ny = y + 1;
    met_opposite = false;
    while(OnBoard(nx, ny)){
        if(board[nx][ny] == 0){
            defended_squares[nx][ny] = true;
        }
        else if(Opposite(turn, board[nx][ny])){
            defended_squares[nx][ny] = true;
            break;
        }
        else if(Same(turn, board[nx][ny])){
            if(!met_opposite){
                met_opposite = true;
                first_opposite = {nx, ny};
                if(board[nx][ny] == (turn ? 6 : -6)){
                    checks.push_back({DIR::DR, Coords{x, y}});
                    break;
                }
            }
            else{
                if(board[nx][ny] == (turn ? 6 : -6)){
                    allowed_moves[first_opposite.x][first_opposite.y].push_back(DIR::UL);
                }
                break;
            }
        }
        ++nx; ++ny;
    }

    nx = x - 1, ny = y + 1;
    met_opposite = false;
    while(OnBoard(nx, ny)){
        if(board[nx][ny] == 0){
            defended_squares[nx][ny] = true;
        }
        else if(Opposite(turn, board[nx][ny])){
            defended_squares[nx][ny] = true;
            break;
        }
        else if(Same(turn, board[nx][ny])){
            if(!met_opposite){
                met_opposite = true;
                first_opposite = {nx, ny};
                if(board[nx][ny] == (turn ? 6 : -6)){
                    checks.push_back({DIR::DL, Coords{x, y}});
                    break;
                }
            }
            else{
                if(board[nx][ny] == (turn ? 6 : -6)){
                    allowed_moves[first_opposite.x][first_opposite.y].push_back(DIR::UR);
                }
                break;
            }
        }
        --nx; ++ny;
    }
}

void Board::CalculateInfoRook(uint_fast8_t x, uint_fast8_t y) noexcept
{
    uint_fast8_t nx = x - 1, ny = y;
    bool met_opposite = false;
    Coords first_opposite;
    while(OnBoard(nx, ny)){
        if(board[nx][ny] == 0){
            defended_squares[nx][ny] = true;
        }
        else if(Opposite(turn, board[nx][ny])){
            defended_squares[nx][ny] = true;
            break;
        }
        else if(Same(turn, board[nx][ny])){
            if(!met_opposite){
                met_opposite = true;
                first_opposite = {nx, ny};
                if(board[nx][ny] == (turn ? 6 : -6)){
                    checks.push_back({DIR::L, Coords{x, y}});
                    break;
                }
            }
            else{
                if(board[nx][ny] == (turn ? 6 : -6)){
                    allowed_moves[first_opposite.x][first_opposite.y].push_back(DIR::R);
                }
                break;
            }
        }
        --nx;
    }

    nx = x + 1;
    met_opposite = false;
    while(OnBoard(nx, ny)){
        if(board[nx][ny] == 0){
            defended_squares[nx][ny] = true;
        }
        else if(Opposite(turn, board[nx][ny])){
            defended_squares[nx][ny] = true;
            break;
        }
        else if(Same(turn, board[nx][ny])){
            if(!met_opposite){
                met_opposite = true;
                first_opposite = {nx, ny};
                if(board[nx][ny] == (turn ? 6 : -6)){
                    checks.push_back({DIR::R, Coords{x, y}});
                    break;
                }
            }
            else{
                if(board[nx][ny] == (turn ? 6 : -6)){
                    allowed_moves[first_opposite.x][first_opposite.y].push_back(DIR::L);
                }
                break;
            }
        }
        ++nx;
    }

    nx = x; ny = y - 1;
    met_opposite = false;
    while(OnBoard(nx, ny)){
        if(board[nx][ny] == 0){
            defended_squares[nx][ny] = true;
        }
        else if(Opposite(turn, board[nx][ny])){
            defended_squares[nx][ny] = true;
            break;
        }
        else if(Same(turn, board[nx][ny])){
            if(!met_opposite){
                met_opposite = true;
                first_opposite = {nx, ny};
                if(board[nx][ny] == (turn ? 6 : -6)){
                    checks.push_back({DIR::U, Coords{x, y}});
                    break;
                }
            }
            else{
                if(board[nx][ny] == (turn ? 6 : -6)){
                    allowed_moves[first_opposite.x][first_opposite.y].push_back(DIR::D);
                }
                break;
            }
        }
        --ny;
    }

    ny = y + 1;
    met_opposite = false;
    while(OnBoard(nx, ny)){
        if(board[nx][ny] == 0){
            defended_squares[nx][ny] = true;
        }
        else if(Opposite(turn, board[nx][ny])){
            defended_squares[nx][ny] = true;
            break;
        }
        else if(Same(turn, board[nx][ny])){
            if(!met_opposite){
                met_opposite = true;
                first_opposite = {nx, ny};
                if(board[nx][ny] == (turn ? 6 : -6)){
                    checks.push_back({DIR::D, Coords{x, y}});
                    break;
                }
            }
            else{
                if(board[nx][ny] == (turn ? 6 : -6)){
                    allowed_moves[first_opposite.x][first_opposite.y].push_back(DIR::U);
                }
                break;
            }
        }
        ++ny;
    }
}

void Board::CalculateInfoQueen(uint_fast8_t x, uint_fast8_t y) noexcept
{
    CalculateInfoBishop(x, y);
    CalculateInfoRook(x, y);
}

void Board::CalculateInfoKing(uint_fast8_t x, uint_fast8_t y) noexcept
{
    if(OnBoard(x - 1, y - 1)){
        defended_squares[x - 1][y - 1] = true;
    }
    if(OnBoard(x - 1, y + 1)){
        defended_squares[x - 1][y + 1] = true;
    }
    if(OnBoard(x + 1, y + 1)){
        defended_squares[x + 1][y + 1] = true;
    }
    if(OnBoard(x + 1, y - 1)){
        defended_squares[x + 1][y - 1] = true;
    }
    if(OnBoard(x, y + 1)){
        defended_squares[x][y + 1] = true;
    }
    if(OnBoard(x, y - 1)){
        defended_squares[x][y - 1] = true;
    }
    if(OnBoard(x + 1, y)){
        defended_squares[x + 1][y] = true;
    }
    if(OnBoard(x - 1, y)){
        defended_squares[x - 1][y] = true;
    }
}

//Function checks if square value is opposite to current turns color, returns true if so, false otherwise (including if square's empty)
bool Board::Opposite(bool turn, int_fast8_t square) const noexcept
{
    if(turn && square < 0){
        return true;
    }
    else if(!turn && square > 0){
        return true;
    }
    return false;
}

bool Board::Same(bool turn, int_fast8_t square) const noexcept
{
    return !Opposite(turn, square);
}

void Board::ClearAllowedMoves() noexcept
{
    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            allowed_moves[i][j].clear();
        }
    }
}

void Board::ClearDefendedSquares() noexcept
{
    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            defended_squares[i][j] = false;
        }
    }
}

QPixmap Board::GetPiecePixmap(int_fast8_t type) noexcept
{
    if(type == 1){
        return QPixmap(":/White_Pawn.png");
    }
    else if(type == -1){
        return QPixmap(":/Black_Pawn.png");
    }
    else if(type == 2){
        return QPixmap(":/White_Knight.png");
    }
    else if(type == -2){
        return QPixmap(":/Black_Knight.png");
    }
    else if(type == 3){
        return QPixmap(":/White_Bishop.png");
    }
    else if(type == -3){
        return QPixmap(":/Black_Bishop.png");
    }
    else if(type == 4){
        return QPixmap(":/White_Rook.png");
    }
    else if(type == -4){
        return QPixmap(":/Black_Rook.png");
    }
    else if(type == 5){
        return QPixmap(":/White_Queen.png");
    }
    else if(type == -5){
        return QPixmap(":/Black_Queen.png");
    }
    else if(type == 6){
        return QPixmap(":/White_King.png");
    }
    else if(type == -6){
        return QPixmap(":/Black_King.png");
    }
    return QPixmap();
}

bool Board::CheckIfSquareDefended(uint_fast8_t x, uint_fast8_t y) const noexcept
{
    return defended_squares[x][y];
}

bool Board::CheckIfMoveIsAllowed(uint_fast8_t x, uint_fast8_t y, Direction::Dir &dir) const noexcept
{
    if(allowed_moves[x][y].empty()){
        return true;
    }
    else if(allowed_moves[x][y].size() > 1){
        return false;
    }

    if(dir == allowed_moves[x][y].front()){
        return true;
    }
    return false;
}

bool Board::IsCheck() const noexcept
{
    return !checks.empty();
}

uint_fast8_t Board::ChecksAmount() const noexcept
{
    return checks.size();
}

//NOTE: if idx is invalid, it'll cause runtime error since there's no check for idx being out of checks array range
std::pair<DIR, Coords>& Board::GetCheckByIdx(uint_fast8_t idx) noexcept
{
    //return const_cast<std::pair<Direction::Dir, Coords>&>(checks[idx]);
    return checks[idx];
}

bool Board::IsCheckmate() const noexcept
{
    return IsCheck() && possible_moves.empty();
}

bool Board::CheckIfPieceWakable(int x, int y) const noexcept
{
    return Same(turn, board[x][y]);
}

bool Board::IsLegalMove(int x1, int y1, int x2, int y2) const noexcept
{
    for(const auto& move : possible_moves){
        if(move.x1 == x1 && move.y1 == y1 && move.x2 == x2 && move.y2 == y2){
            return true;
        }
    }
    return false;
}

void Board::ApplyUserMove(int x1, int y1, int x2, int y2)
{
    Move user_move(board[x1][y1], x1, y1, x2, y2, board[x2][y2]);
    for(auto& move : possible_moves){
        if(move.x1 == x1 && move.y1 == y1 && move.x2 == x2 && move.y2 == y2){
            RunMove(move);
            break;
        }
    }
}

void Board::RunMove(Move &move)
{
    //En passant case
    if(move.ep_x != 8){
        board[move.ep_x][move.ep_y] = 0;
    }

    if(turn){
        if(!white_king_moved && move.piece == 6){
            white_king_moved = true;
        }
        if(move.piece == 4){
            if(!white_kingside_rook_moved && move.x1 == 7){
                white_kingside_rook_moved = true;
            }
            if(!white_queenside_rook_moved && move.x1 == 0){
                white_queenside_rook_moved = true;
            }
        }
    }
    else{
        if(!black_king_moved && move.piece == -6){
            black_king_moved = true;
        }
        if(move.piece == -4){
            if(!black_kingside_rook_moved && move.x1 == 7){
                black_kingside_rook_moved = true;
            }
            if(!black_queenside_rook_moved && move.x1 == 0){
                black_queenside_rook_moved = true;
            }
        }
    }

    if(move.castle){
        if(turn && move.x2 == 6){
            white_kingside_rook_moved = true;
            white_king_moved = true;
            board[7][7] = 0;
            board[5][7] = 4;
        }
        else if(turn && move.x2 == 2){
            white_queenside_rook_moved = true;
            white_king_moved = true;
            board[0][7] = 0;
            board[3][7] = 4;
        }
        else if(!turn && move.x2 == 2){
            black_queenside_rook_moved = true;
            black_king_moved = true;
            board[7][0] = 0;
            board[5][0] = -4;
        }
        else if(!turn && move.x2 == 6){
            black_kingside_rook_moved = true;
            black_king_moved = true;
            board[0][0] = 0;
            board[3][0] = -4;
        }
    }

    if(abs(move.piece) == 6){
        (turn ? white_king_coords : black_king_coors) = {move.x2, move.y2};
    }

    board[move.x2][move.y2] = board[move.x1][move.y1];
    board[move.x1][move.y1] = 0;

    total_turns++;
    turn = !turn;
    moves_played.push_back(move);

    CalculatePossibleMoves();
}

Direction::Dir Direction::CalculateDir(Coords from, Coords to) noexcept
{
    int_fast8_t x_diff = from.x - to.x;
    int_fast8_t y_diff = from.y - to.y;
    //UL
    if(x_diff > 0 && y_diff > 0 && x_diff == y_diff){
        return DIR::UL;
    }
    //U
    if(x_diff == 0 && y_diff > 0){
        return DIR::U;
    }
    //UR
    if(x_diff < 0 && y_diff > 0 && -x_diff == y_diff){
        return DIR::UR;
    }
    //R
    if(x_diff < 0 && y_diff == 0){
        return DIR::R;
    }
    //DR
    if(x_diff < 0 && y_diff < 0 && x_diff == y_diff){
        return DIR::DR;
    }
    //D
    if(x_diff == 0 && y_diff < 0){
        return DIR::D;
    }
    //DL
    if(x_diff > 0 && y_diff < 0 && x_diff == -y_diff){
        return DIR::DL;
    }
    //L
    if(x_diff > 0 && y_diff == 0){
        return DIR::L;
    }
    //from == to
    if(x_diff == 0 && y_diff == 0){
        return DIR::NONE;
    }

    //Everything else is considered a knight move
    return DIR::KM;
}

//Returns opposite direction if type of direction is diagonal, horizontal or vertical otherwise returns given direction
Direction::Dir Direction::Opposite(Dir &dir) noexcept
{
    if(dir == DIR::D){
        return DIR::U;
    }
    if(dir == DIR::DL){
        return DIR::UR;
    }
    if(dir == DIR::DR){
        return DIR::UL;
    }
    if(dir == DIR::L){
        return DIR::R;
    }
    if(dir == DIR::R){
        return DIR::R;
    }
    if(dir == DIR::U){
        return DIR::D;
    }
    if(dir == DIR::UL){
        return DIR::DR;
    }
    if(dir == DIR::UR){
        return DIR::DL;
    }

    return dir;
}

bool Coords::Valid() const noexcept
{
    if(x > 7 || y > 7){
        return false;
    }
    return true;
}
