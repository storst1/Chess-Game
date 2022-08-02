#include "board.h"
#include "engine/engine.h"

Board::Board() noexcept : possible_moves(*this)
{
    possible_moves.reserve(possible_moves_capacity);
    moves_played.reserve(moves_played_capacity);
    allowed_moves.resize(8, std::vector<std::vector<DIR>> (8, std::vector<DIR>(0)));
}

Board::Board(const Board &parent) noexcept
    : turn(parent.turn),
      total_turns(parent.total_turns),
      possible_moves(parent.possible_moves),
      allowed_moves(parent.allowed_moves),
      moves_played(parent.moves_played),
      black_king_moved(parent.black_king_moved),
      white_king_moved(parent.white_king_moved),
      black_kingside_rook_moved(parent.black_kingside_rook_moved),
      black_queenside_rook_moved(parent.black_queenside_rook_moved),
      white_kingside_rook_moved(parent.white_kingside_rook_moved),
      white_queenside_rook_moved(parent.white_queenside_rook_moved),
      checks(parent.checks),
      black_king_coors(parent.black_king_coors),
      white_king_coords(parent.white_king_coords)
{
    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            board[i][j] = parent.board[i][j];
            defended_squares[i][j] = parent.defended_squares[i][j];
        }
    }
}

//Loads position from Forsyth–Edwards Notation
//Returns true if given string was valid and process was successfull, false otherwise
bool Board::LoadFromFEN(const QString &FEN)
{
    //Pre-setting bool variables that hold castle rights to true (default value)
    //since they'll be set false by the FEN string if needed
    SetAllCastleVars(true);

    enum ReadingStage{board_pos, cur_turn, castle_rights, en_passant_file,
                      halfmove_clock, fullmove_clock, end}; //last three are considered not necessary
    auto IncrStage = [](ReadingStage& stage){
        if(stage == fullmove_clock){
            return; //Stage cannot go further than "fullmove_clock" stage
        }
        //Uses the assumption that ReadingStage enum is continuous
        stage = static_cast<ReadingStage>(static_cast<int>(stage) + 1);
    };
    ReadingStage stage = board_pos;
    bool skip_letter = false;
    int file = 0;
    int square = 0;
    for(const QChar& c : FEN){
        if(skip_letter){
            skip_letter = false;
            continue;
        }
        if(c.isLetter() || c == '-'){
            if(stage == board_pos){
                if(!OnBoard(file, square)){
                    return false;
                }

                uint_fast8_t value = 0;
                if(c.toLower() == 'p'){
                    value = 1;
                }
                else if(c.toLower() == 'n'){
                    value = 2;
                }
                else if(c.toLower() == 'b'){
                    value = 3;
                }
                else if(c.toLower() == 'r'){
                    value = 4;
                }
                else if(c.toLower() == 'q'){
                    value = 5;
                }
                else if(c.toLower() == 'k'){
                    value = 6;
                }
                else{
                    //Current tetter is not a piece, FEN code is invalid
                    return false;
                }
                board[square][file] = (c.isUpper() ? value : -value);
                square++;
            }
            else if(stage == cur_turn){
                if(c == 'w'){
                    turn = true;
                }
                else if(c == 'b'){
                    turn = false;
                }
                else{
                    //Letter does not describe current move holder
                    return false;
                }
                IncrStage(stage);
                skip_letter = true;
            }
            else if(stage == castle_rights){
                if(c == 'k'){
                    black_king_moved = false;
                    black_kingside_rook_moved = false;
                }
                else if(c == 'q'){
                    black_king_moved = false;
                    black_queenside_rook_moved = false;
                }
                else if(c == 'K'){
                    white_king_moved = false;
                    white_kingside_rook_moved = false;
                }
                else if(c == 'Q'){
                    white_king_moved = false;
                    white_queenside_rook_moved = false;
                }
                else if(c == '-'){
                    continue;
                }
                else{
                    return false;
                }
            }
            else if(stage == en_passant_file){
                if(c == '-'){
                    skip_letter = true;
                    IncrStage(stage);
                    continue;
                }
                else{
                    return false;
                }
            }
            else if(stage == halfmove_clock || stage == fullmove_clock){
                //There shouldn't be letters on the en_passant_file stage
                return false;
            }
        }
        else if(c == '/'){
            if(stage != board_pos){
                return false;
            }

            file++;
            square = 0;
        }
        else if(c.isDigit()){
            if(stage == board_pos){
                square += (c.toLatin1() - '0');
                if(square > 8){
                    return false;
                }
            }
            else if(stage == en_passant_file){
                //TO DO: do something about it
            }
            else if(stage == halfmove_clock){
                //TO DO: do something about it
            }
            else if(stage == fullmove_clock){
                //TO DO: do something about it
            }
            else{
                return false;
            }
        }
        else if(c == ' '){
            IncrStage(stage);
        }
        else{
            //Unknown symbol
            return false;
        }
    }
    if(stage == board_pos || stage == cur_turn || stage == castle_rights || stage == en_passant_file){
        return false;
    }
    return true;
}

//This function draws the board to the @*painter object of QPainter type
void Board::DrawBoard(QPainter *painter, int cx, int cy) const noexcept
{
    bool painted[8][8]{};

    if(IsCheck()){
        Coords king_pos = KingPos(turn);
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

//Returns reference to the last move that was played on this board
//Will throw exception if no moves were played
Move &Board::LastMove()
{
    if(moves_played.empty()){
        throw std::out_of_range("Cannot get a last move in Board::LastMove() function since no moves was played");
    }
    return moves_played[moves_played.size() - 1];
}

//Returns current position of the king of given @color based on the value of the internal variable
//This function might return wrong result if board was initialized with non-standart starting position
Coords Board::KingPos(bool color) const noexcept
{
    return (color ? white_king_coords : black_king_coors);
}

//Iterates over the board and returns Coords of the king of given @color
//If king of given @color is not on the board, returns Coords{8, 8}
Coords Board::FindKing(bool color) const noexcept
{
    for(uint_fast8_t i = 0; i < 8; ++i){
        for(uint_fast8_t j = 0; j < 8; ++j){
            if(board[i][j] == (color ? 6 : -6)){
                return Coords{i, j};
            }
        }
    }
    return Coords{8, 8};
}

//Does not check if pair {x, y} is on the board
bool Board::IsDefended(uint_fast8_t x, uint_fast8_t y) const noexcept
{
    return defended_squares[x][y];
}

//Calculates all the possible moves on the board for the color that ows the current turn
//Reuslts are being pushed back to the possible_moves vector property
void Board::CalculateAllMoves() noexcept
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

//This method checks if given pair (x, y) represents valid coordinates on the board
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
    CalculateSlidingMoves(x, y, -1, -1);
    CalculateSlidingMoves(x, y, -1, 1);
    CalculateSlidingMoves(x, y, 1, -1);
    CalculateSlidingMoves(x, y, 1, 1);
}

void Board::CalculateRookMoves(uint_fast8_t x, uint_fast8_t y) noexcept
{
    CalculateSlidingMoves(x, y, -1, 0);
    CalculateSlidingMoves(x, y, 1, 0);
    CalculateSlidingMoves(x, y, 0, -1);
    CalculateSlidingMoves(x, y, 0, 1);
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

//This function generates moves for the any type of sliding piece in given direction (@x_del, @y_del)
void Board::CalculateSlidingMoves(const uint_fast8_t x, const uint_fast8_t y, int_fast8_t x_del, int_fast8_t y_del) noexcept
{
    uint_fast8_t nx = x + x_del, ny = y + y_del;
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
        nx += x_del;
        ny += y_del;
    }
}

//Calculates additional info from the opponent pieces that might be used during move generation of own pieces
//This function gathers information such as:
// - Defended squares (fills in @defended_squares property) - squares that are being attacked/protected/under contol of opponent pieces
// - Checks on the board (@checks property) and type of those checks
// - Allowed moves (fills in @allowed_moves property) - 2D vector to keep track of allowed move directions in case of check
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
            if(board[x - 1][y + 1] == 6){
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
    CalculateInfoSlidingTowards(x, y, -1, -1, DIR::UL, DIR::DR);
    CalculateInfoSlidingTowards(x, y, -1, 1, DIR::DL, DIR::UR);
    CalculateInfoSlidingTowards(x, y, 1, -1, DIR::UR, DIR::DL);
    CalculateInfoSlidingTowards(x, y, 1, 1, DIR::DR, DIR::UL);
}

void Board::CalculateInfoRook(uint_fast8_t x, uint_fast8_t y) noexcept
{
    CalculateInfoSlidingTowards(x, y, -1, 0, DIR::L, DIR::R);
    CalculateInfoSlidingTowards(x, y, 1, 0, DIR::R, DIR::L);
    CalculateInfoSlidingTowards(x, y, 0, -1, DIR::U, DIR::D);
    CalculateInfoSlidingTowards(x, y, 0, 1, DIR::D, DIR::U);
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

void Board::CalculateInfoSlidingTowards(const uint_fast8_t x, const uint_fast8_t y,
                                       int_fast8_t x_del, int_fast8_t y_del,
                                       const DIR dir, DIR opp_dir) noexcept
{
    uint_fast8_t nx = x + x_del, ny = y + y_del;
    if(opp_dir == DIR::NONE){ //If opp_dir set to default value, we need to calculate its actual value
        opp_dir = Direction::Opposite(dir);
    }
    bool met_opposite = false;
    Coords first_opposite;
    while(OnBoard(nx, ny)){
        if(board[nx][ny] == 0 && !met_opposite){
            defended_squares[nx][ny] = true;
        }
        else if(Opposite(turn, board[nx][ny])){
            if(!met_opposite){
                defended_squares[nx][ny] = true;
            }
            break;
        }
        else if(Same(turn, board[nx][ny])){
            if(!met_opposite){
                met_opposite = true;
                first_opposite = {nx, ny};
                if(board[nx][ny] == (turn ? 6 : -6)){
                    checks.push_back({dir, Coords{x, y}});
                    break;
                }
            }
            else{
                if(board[nx][ny] == (turn ? 6 : -6)){
                    allowed_moves[first_opposite.x][first_opposite.y].push_back(opp_dir);
                }
                break;
            }
        }
        nx += x_del;
        ny += y_del;
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

//Function checks if square value is same to current turns color, returns true if so, false otherwise (excluding if square's empty)
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

//Sets all the variables that are used to determine if each type of castle is possible to @value
void Board::SetAllCastleVars(bool value) noexcept
{
    white_king_moved = value;
    black_king_moved = value;

    white_kingside_rook_moved = value;
    white_queenside_rook_moved = value;
    black_kingside_rook_moved = value;
    black_queenside_rook_moved = value;
}

//Returns QPixmap for the given @type of piece (might vary from -6 to 6)
//Will return output of default QPixmap constructor (QPixmap()) if:
//   @type > 6
//or @type < -6
//or @type == 0
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

//Returns character that is used to describe given @file
//Ex: 0 - 'a', 1 - 'b', etc.
//Returns '?' if file argument is incorrect (does not adhere to the expression -1 < @file < 8)
QChar Board::FileToChar(uint_fast8_t file) noexcept
{
    if(file > 7){
        //Since the type of @file is unsigned, we don't need to check if @file < 0
        return '?';
    }
    return QChar(file + 49);
}

bool Board::CheckIfSquareDefended(uint_fast8_t x, uint_fast8_t y) const noexcept
{
    return defended_squares[x][y];
}

//Does NOT check if move is allowed in general!
//Checks if move from {@x, @y} in given direction(@dir) is possible during check based on the pre-calculated
//content of the @allowed_moves property
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

//Returns true if there're any checks on board, false otherwise
bool Board::IsCheck() const noexcept
{
    return !checks.empty();
}

//Returns amount of checks there are currently on the board
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

bool Board::WCK_Possible() const noexcept
{
    return white_king_moved && white_kingside_rook_moved;
}

bool Board::WCQ_Possible() const noexcept
{
    return white_king_moved && white_queenside_rook_moved;
}

bool Board::BCK_Possible() const noexcept
{
    return black_king_moved && black_kingside_rook_moved;
}

bool Board::BCQ_Possible() const noexcept
{
    return black_king_moved && black_queenside_rook_moved;
}

//Returns board to the default state except without any pieces on it (all the squares are empty)
void Board::ClearEverything() noexcept
{
    ClearBoardPieces();
    SetAllCastleVars(false);
    turn = true;
    checks.clear();
    possible_moves.clear();
    total_turns = 0;
    moves_played.clear();
    ClearDefendedSquares();
    black_king_coors = {4, 0}; //Default value, but not necessarily correct
    white_king_coords = {4, 7}; //Default value, but not necessarily correct
}

void Board::ClearBoardPieces() noexcept
{
    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            board[i][j] = 0;
        }
    }
}

//Sets board to the default starting position using Board::LoadFromFEN() function
//Could be used to reset the board in runtime
void Board::SetDefaultStartingBoard() noexcept
{
    const QString starting_pos_FEN_str = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    LoadFromFEN(starting_pos_FEN_str);
}

size_t Board::PossibleMovesStartingCapacity() const noexcept
{
    return possible_moves_capacity;
}

size_t Board::MovesPlayedStartingCapacity() const noexcept
{
    return moves_played_capacity;
}

int Board::TotalBishops() const noexcept
{
    int cnt = 0;
    for(size_t i = 0; i < 64; ++i){
        if(abs(board[i % 8][i / 8]) == 3){
            ++cnt;
        }
    }
    return cnt;
}

int Board::TotalPawns() const noexcept
{
    int cnt = 0;
    for(size_t i = 0; i < 64; ++i){
        if(abs(board[i % 8][i / 8]) == 1){
            ++cnt;
        }
    }
    return cnt;
}

int Board::TotalKnights() const noexcept
{
    int cnt = 0;
    for(size_t i = 0; i < 64; ++i){
        if(abs(board[i % 8][i / 8]) == 2){
            ++cnt;
        }
    }
    return cnt;
}

int Board::TotalRooks() const noexcept
{
    int cnt = 0;
    for(size_t i = 0; i < 64; ++i){
        if(abs(board[i % 8][i / 8]) == 4){
            ++cnt;
        }
    }
    return cnt;
}

int Board::TotalQueens() const noexcept
{
    int cnt = 0;
    for(size_t i = 0; i < 64; ++i){
        if(abs(board[i % 8][i / 8]) == 5){
            ++cnt;
        }
    }
    return cnt;
}

int Board::TotalKings() const noexcept
{
    int cnt = 0;
    for(size_t i = 0; i < 64; ++i){
        if(abs(board[i % 8][i / 8]) == 6){
            ++cnt;
        }
    }
    return cnt;
}

int Board::TotalEmptySquares() const noexcept
{
    int cnt = 0;
    for(size_t i = 0; i < 64; ++i){
        if(board[i % 8][i / 8] == 0){
            ++cnt;
        }
    }
    return cnt;
}

int Board::TotalBishops(bool color) const noexcept
{
    int cnt = 0;
    for(size_t i = 0; i < 64; ++i){
        if(board[i % 8][i / 8] == (color ? 3 : -3)){
            ++cnt;
        }
    }
    return cnt;
}

int Board::TotalPawns(bool color) const noexcept
{
    int cnt = 0;
    for(size_t i = 0; i < 64; ++i){
        if(board[i % 8][i / 8] == (color ? 1 : -1)){
            ++cnt;
        }
    }
    return cnt;
}

int Board::TotalKnights(bool color) const noexcept
{
    int cnt = 0;
    for(size_t i = 0; i < 64; ++i){
        if(board[i % 8][i / 8] == (color ? 2 : -2)){
            ++cnt;
        }
    }
    return cnt;
}

int Board::TotalRooks(bool color) const noexcept
{
    int cnt = 0;
    for(size_t i = 0; i < 64; ++i){
        if(board[i % 8][i / 8] == (color ? 4 : -4)){
            ++cnt;
        }
    }
    return cnt;
}

int Board::TotalQueens(bool color) const noexcept
{
    int cnt = 0;
    for(size_t i = 0; i < 64; ++i){
        if(board[i % 8][i / 8] == (color ? 5 : -5)){
            ++cnt;
        }
    }
    return cnt;
}

int Board::TotalKings(bool color) const noexcept
{
    int cnt = 0;
    for(size_t i = 0; i < 64; ++i){
        if(board[i % 8][i / 8] == (color ? 6 : -6)){
            ++cnt;
        }
    }
    return cnt;
}

int Board::TotalPieces() const noexcept
{
    int cnt = 0;
    for(size_t i = 0; i < 64; ++i){
        if(board[i % 8][i / 8]){
            ++cnt;
        }
    }
    return cnt;
}

int Board::TotalPieces(bool color) const noexcept
{
    int color_sgn = color;
    if(!color_sgn){
        color_sgn = -color_sgn;
    }
    int cnt = 0;
    for(size_t i = 0; i < 64; ++i){
        if(PieceValue::Sgn(board[i % 8][i / 8]) == color_sgn){
            ++cnt;
        }
    }
    return cnt;
}

//Checks if piece on square {@x, @y} could be used by owner of current turn
//(in other words: checks if it's piece of the same color that owns the turn)
bool Board::CheckIfPieceWakable(int x, int y) const noexcept
{
    return Same(turn, board[x][y]);
}

//Checks if move from {@x1, @y1} square to {@x2, @y2} square could be played in current position
//NOTE: This function does not generate moves, it only checks for move in @possible_moves vector property
//So it might return invalid result if moves weren't generated beforehand
bool Board::IsLegalMove(int x1, int y1, int x2, int y2) const noexcept
{
    for(const auto& move : possible_moves){
        if(move.x1 == x1 && move.y1 == y1 && move.x2 == x2 && move.y2 == y2){
            return true;
        }
    }
    return false;
}

//Moves piece from {@x1, @y1} square to {@x2, @y2} square with prior check if move is legal
//If such move is not legal, will do nothing
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

PossibleMovesVector &Board::PossibleMovesRef() noexcept
{
    return possible_moves;
}

PossibleMovesVector Board::PossibleMoves() const noexcept
{
    return possible_moves;
}

//Performs @move on the board fully assuming given @move is legal
void Board::RunMove(const Move &move)
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

    ++total_turns;
    turn = !turn;
    moves_played.push_back(move);

    CalculateAllMoves();
}

//Reverts last move assuming that that it does excist without checking for it
void Board::RevertLastMove() noexcept
{
    Move& move = moves_played.back();

    //En passant case
    if(move.ep_x != 8){
        board[move.ep_x][move.ep_y] = move.captured;
    }

    if(!turn){
        if(!white_king_moved && move.piece == 6){
            white_king_moved = false;
        }
        if(move.piece == 4){
            if(!white_kingside_rook_moved && move.x1 == 7){
                white_kingside_rook_moved = false;
            }
            if(!white_queenside_rook_moved && move.x1 == 0){
                white_queenside_rook_moved = false;
            }
        }
    }
    else{
        if(!black_king_moved && move.piece == -6){
            black_king_moved = false;
        }
        if(move.piece == -4){
            if(!black_kingside_rook_moved && move.x1 == 7){
                black_kingside_rook_moved = false;
            }
            if(!black_queenside_rook_moved && move.x1 == 0){
                black_queenside_rook_moved = false;
            }
        }
    }

    if(move.castle){
        if(!turn && move.x2 == 6){
            white_kingside_rook_moved = false;
            white_king_moved = false;
            board[7][7] = 4;
            board[5][7] = 0;
        }
        else if(!turn && move.x2 == 2){
            white_queenside_rook_moved = false;
            white_king_moved = false;
            board[0][7] = 4;
            board[3][7] = 0;
        }
        else if(turn && move.x2 == 2){
            black_queenside_rook_moved = false;
            black_king_moved = false;
            board[7][0] = -4;
            board[5][0] = 0;
        }
        else if(turn && move.x2 == 6){
            black_kingside_rook_moved = false;
            black_king_moved = false;
            board[0][0] = -4;
            board[3][0] = 0;
        }
    }

    if(abs(move.piece) == 6){
        (!turn ? white_king_coords : black_king_coors) = {move.x1, move.y1};
    }

    board[move.x1][move.y1] = board[move.x2][move.y2];
    board[move.x2][move.y2] = move.captured;

    --total_turns;
    turn = !turn;
    moves_played.pop_back();

    CalculateAllMoves();
}

std::vector<Move> Board::PlayedMoves() const noexcept
{
    return moves_played;
}

std::vector<Move> &Board::PlayedMovesRef() noexcept
{
    return moves_played;
}

//Prints the content of the @defended_sqaures property in qDebug()
void Board::D_PrintDefendedSquares() const noexcept
{
    QDebug debug = qDebug();
    debug << "Defended squares:\n";
    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            debug << (int)defended_squares[j][i] << " ";
        }
        debug << "\n";
    }
}

//Prints the content of the @board property in qDebug()
void Board::D_PrintBoard() const noexcept
{
    QDebug debug = qDebug();
    debug << "Board:\n ";
    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            debug << (int)board[i][j] << " ";
        }
        debug << "\n";
    }
}
