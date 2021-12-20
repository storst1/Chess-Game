#include "board.h"
#include "AI.h"
#include "BetterAI.h"

#include <QDebug>
#include "DebugOutput.h"

BOARD* board;
PAWN *pawn;
KNIGHT *knight;
BISHOP *bishop;
ROOK *rook;
QUEEN *queen;
KING *king;

extern Main_Tree* AI_MAIN_TREE;

extern int board_x;
extern int board_y;
extern bool AI_used;
extern bool AI_turn;

extern MainWindow win;
extern QColor* Black_Square;
extern QColor* White_Square;
extern QColor* Highlighted_Square;
extern QColor* Possibly_Highlighted_Square;
extern QColor* Check_Highlighted_Square;
extern QColor* Last_Move_Highlight;

QVector<QVector<float>> c_p;
QVector<QVector<float>> c_kn;
QVector<QVector<float>> c_b;
QVector<QVector<float>> c_r;
QVector<QVector<float>> c_q;
QVector<QVector<float>> c_king;

const float FINF = 1e9f;

void create_board_objects(){
    //Creates main objects that have global initialization
    pawn = new PAWN();
    knight = new KNIGHT();
    bishop = new BISHOP();
    rook = new ROOK();
    queen = new QUEEN();
    king = new KING();
    board = new BOARD();
    load_from_file(1, c_p);
    load_from_file(2, c_kn);
    load_from_file(3, c_b);
    load_from_file(4, c_r);
    load_from_file(5, c_q);
    load_from_file(6, c_king);
    const std::string starting_position = "brbkbbbqbKbbbkbrbpbpbpbpbpbpbpbp32wpwpwpwpwpwpwpwpwrwkwbwqwKwbwkwr";
    board->setup_board(starting_position, true);
}

int convert_piece_type_to_int(char PT){
    switch(PT){
    case 'p':
        return 1;
    case 'k':
        return 2;
    case 'b':
        return 3;
    case 'r':
        return 4;
    case 'q':
        return 5;
    case 'K':
        return 6;
    }
    return -1;
}

char convert_piece_type_to_char(int PT){
    if(PT == 1) return 'p';
    else if(PT == 2) return 'k';
    else if(PT == 3) return 'b';
    else if(PT == 4) return 'r';
    else if(PT == 5) return 'q';
    else if(PT == 6) return 'K';
    return ' ';
}

std::string move_to_string(MOVE move, BOARD board){
    std::string result;
    bool captured = false;
    int move_type = move.get_move_type(&board);
    if(move_type == 1){
        //Queen side castle
        return "O-O-O";
    }
    else if(move_type == 2){
        //King side castle
        return "O-O";
    }
    else if(move_type == 3){
        //En passant
        captured = true;
    }
    //From
    result += std::to_string(move.get_from()->X() + 17); //Making a shift with +17 because '0' = 48 and 'A' = 65 in ASCII table
    result += std::to_string(move.get_from()->Y());
    if(!move.get_to()->empty() && move.get_to()->get_piece()->is_white() != move.get_from()->get_piece()->is_white()){
        //Capture
        captured = true;
    }
    if(captured){
        result += "x";
    }
    //To
    result += std::to_string(move.get_from()->X() + 17); //Making a shift with +17 because '0' = 48 and 'A' = 65 in ASCII table
    result += std::to_string(move.get_from()->Y());
    return result;
}

PIECE::PIECE(int type, bool color):type(type), color(color){
    moved = false;
    if(type == 1) //pawn
        Img = (color ? pawn->WHITE() : pawn->BLACK());
    else if(type == 2) //knight
        Img = (color ? knight->WHITE() : knight->BLACK());
    else if(type == 3) //bishop
        Img = (color ? bishop->WHITE() : bishop->BLACK());
    else if(type == 4) //rook
        Img = (color ? rook->WHITE() : rook->BLACK());
    else if(type == 5) //queen
        Img = (color ? queen->WHITE() : queen->BLACK());
    else if(type == 6) //king
        Img = (color ? king->WHITE() : king->BLACK());
    else{} //wrong type, nothing to be done
};

PIECE::~PIECE(){
    //delete Img;
}

QImage* PIECE::get_piece_img(){
    return Img;
}

int PIECE::get_type(){
    return type;
}

char PIECE::get_char_type(){
    //Gets piece type and returns its char name
    return convert_piece_type_to_char(type);
}

PIECE* SQUARE::get_piece(){
    return piece;
}

int PIECE::get_evaluation_value(){
    if(type == 1){ //Pawn
        return 100;
    }
    else if(type == 2){ //Knight
        return 300;
    }
    else if(type == 3){ //Bishop
        return 300;
    }
    else if(type == 4){ //Rook
        return 500;
    }
    else if(type == 5){ //Queen
        return 900;
    }
    else{ //King
        return 400;
    }
    //???
    return -INF;
}


bool SQUARE::on_the_board(){
    //Checks if given coors are on the board
    if(x >= 8 || x < 0 || y < 0 || y >= 8)
        return false;
    return true;
}

bool SQUARE::empty(){
    //Checks if there is a piece on a given square
    if(piece == nullptr)
        return true;
    return false;
}

void SQUARE::assign_piece(int type, bool color){
    piece = new PIECE(type, color);
    //if(piece->get_char_type() != 'K' && piece->get_char_type() != 'r') piece->mark_moved();
}

void SQUARE::assign_piece(PIECE* _piece){
    piece = _piece;
}

void SQUARE::assign_copy_of_piece(PIECE* _piece){
    //Deletes previous piece in the square and instead creates a NEW one which is a COPY of given argument _piece
    delete piece;
    piece = new PIECE(_piece->get_type(), _piece->is_white());
    if(_piece->has_moved()){
        piece->mark_moved();
    }
}

void SQUARE::move_n_squares(int n){
    //Sets SQUARE to position that is n-squares away from current or sets it to {7, 7} if it will be out of the board after moving n times
    int y_move = n / 8;
    int x_move = n % 8;
    if(x + x_move >= 8){
        y_move++;
        x_move = (x + x_move) % 8 - x;
    }
    x += x_move;
    y += y_move;
    if(!on_the_board()){
        //Position is out of the board
        x = 7;
        y = 7;
    }
}

bool SQUARE::is_white(){
    return color;
}

void SQUARE::set_empty(){
    //Deletes a piece from square
    if(piece != nullptr){
        piece = nullptr;
    }
    highlighted = false;
}

bool SQUARE::check_if_move_is_possible(int i, int j){
    //Checks if there is already calculated such move that we will end up in square[i][j] from current square
    for(int u = 0; u < (int)possible_moves.size(); ++u){
        if(possible_moves[u]->X() == i && possible_moves[u]->Y() == j){
            return true;
        }
    }
    return false;
}

void SQUARE::erase_from_possible_moves(int from, int to){
    //no exceptions
    //Erases moves in given range
    possible_moves.erase(possible_moves.begin() + from, possible_moves.begin() + to);
}

SQUARE::~SQUARE(){
    delete piece;
}

MOVE::MOVE(SQUARE* from, SQUARE* to, BOARD* cur_board):from(from), to(to), _board(cur_board){
    from != nullptr ? piece = from->get_piece() : nullptr;
    move_type = get_move_type(cur_board);
}

MOVE::~MOVE(){
}

void BOARD::check_en_passant(QVector<QPair<int, int>> &moves, int x1, int y1, bool color){
    if(index_on_board(x1, y1)){
        SQUARE* pos_square = (color ? b[x1][y1 + 1] : b[x1][y1 - 1]);
        if(b[x1][y1]->empty() && !pos_square->empty() && pos_square->get_piece()->is_white() != color && pos_square->get_piece()->get_type() == 1 && get_last_move().get_to()->X() == x1 && get_last_move().get_to()->Y() == (color ? y1 + 1 : y1 - 1) && get_last_move().get_from()->Y() == (color ? y1 - 1 : y1 + 1)){
            moves.push_back({x1, y1});
        }
    }
}

void BOARD::check_castle(QVector<QPair<int, int>>& moves, int x1, int y1, bool color){
    if(b[x1][y1]->get_piece()->has_moved())
        return;
    //Queen side castle
    if(!b[0][y1]->empty() && b[0][y1]->get_piece()->get_type() == 4 && b[0][y1]->get_piece()->is_white() == color && !b[0][y1]->get_piece()->has_moved()){
        bool possible = true;
        for(int i = 1; i < x1; ++i){
            if(!b[i][y1]->empty() || b[i][y1]->is_protected()){
                possible = false;
                break;
            }
        }
        if(possible){
            QVector<MOVE> all_opponent_moves = get_all_possible_moves(!turn);
            for(int z = 0; z < (int)all_opponent_moves.size(); ++z){
                for(int i = 1; i < x1; ++i){
                    if(b[i][y1] == all_opponent_moves[z].get_to()){
                        possible = false;
                        break;
                    }
                }
                if(!possible){
                    break;
                }
            }
        }
        if(possible){
            moves.push_back({0, y1});
        }
    }
    //King side castle
    if(!b[7][y1]->empty() && b[7][y1]->get_piece()->get_type() == 4 && b[7][y1]->get_piece()->is_white() == color && !b[7][y1]->get_piece()->has_moved()){
        bool possible = true;
        for(int i = x1 + 1; i < 7; ++i){
            if(!b[i][y1]->empty() || b[i][y1]->is_protected()){
                possible = false;
                break;
            }
        }
        if(possible){
            QVector<MOVE> all_opponent_moves = get_all_possible_moves(!turn);
            for(int z = 0; z < (int)all_opponent_moves.size(); ++z){
                for(int i = x1 + 1; i < 7; ++i){
                    if(b[i][y1] == all_opponent_moves[z].get_to()){
                        possible = false;
                        break;
                    }
                }
                if(!possible){
                    break;
                }
            }
        }
        if(possible){
            moves.push_back({7, y1});
        }
    }
}

void BOARD::check_pawn_move(QVector<QPair<int, int>>& moves, int x1, int y1, bool color, bool precalculation, int forward){
    if(index_on_board(x1, y1)){
        if(forward == 1 && get_square(x1, y1)->empty() && !precalculation){
            moves.push_back({x1, y1});
        }
        if(forward == 2 && !get_square(x1, (color ? y1 + 2 : y1 - 2))->get_piece()->has_moved() && get_square(x1, y1)->empty() && get_square(x1, (color ? y1 + 1 : y1 - 1))->empty() && !precalculation){
            moves.push_back({x1, y1});
        }
        if(forward == 0 && !get_square(x1, y1)->empty() && get_square(x1, y1)->get_piece()->is_white() == color && precalculation){
            moves.push_back({x1, y1});
        }
        if(forward == 0 && !get_square(x1, y1)->empty() && get_square(x1, y1)->get_piece()->is_white() != color && !precalculation){
            moves.push_back({x1, y1});
        }
        if(forward == 0 && get_square(x1, y1)->empty() && precalculation){
            moves.push_back({x1, y1});
        }
    }
}

void BOARD::check_knight_move(QVector<QPair<int, int>>& moves, int x1, int y1, bool color, bool precalculation){
    if(index_on_board(x1, y1)){
        if(!get_square(x1, y1)->empty() && get_square(x1, y1)->get_piece()->is_white() == color && precalculation){
            moves.push_back({x1, y1});
        }
        if(get_square(x1, y1)->empty() && !precalculation){
            moves.push_back({x1, y1});
        }
        if(!get_square(x1, y1)->empty() && get_square(x1, y1)->get_piece()->is_white() != color && !precalculation){
            moves.push_back({x1, y1});
        }
    }
}

bool BOARD::check_long_range_move(QVector<QPair<int, int>>* moves, int x1, int y1, bool color, bool precalculation){
    if(index_on_board(x1, y1)){
        if(!get_square(x1, y1)->empty() && get_square(x1, y1)->get_piece()->is_white() == color && precalculation){
            moves->push_back({x1, y1});
            return false;
        }
        if(get_square(x1, y1)->empty() && !precalculation){
            moves->push_back({x1, y1});
            return true;
        }
        if(!get_square(x1, y1)->empty() && get_square(x1, y1)->get_piece()->is_white() != color && !precalculation){
            moves->push_back({x1, y1});
            return false;
        }
        if(!get_square(x1, y1)->empty()){
            return false;
        }
        return true;
    }
    return false;
}

void BOARD::check_king_move(QVector<QPair<int, int>>& moves, int x1, int y1, bool color, bool precalculation){
    if(index_on_board(x1, y1)){
        if(!get_square(x1, y1)->empty() && get_square(x1, y1)->get_piece()->is_white() == color && precalculation){
            moves.push_back({x1, y1});
        }
        if(get_square(x1, y1)->empty() && !precalculation && !get_square(x1, y1)->is_protected()){
            moves.push_back({x1, y1});
        }
        if(!get_square(x1, y1)->empty() && get_square(x1, y1)->get_piece()->is_white() != color && !precalculation && !get_square(x1, y1)->is_protected()){
            moves.push_back({x1, y1});
        }
    }
}

void BOARD::calculate_pawn_moves(QVector<QPair<int, int>>& moves, int x, int y, bool color, bool precalculation){
    int x1, y1;
    //1 square forward
    if(color){
        x1 = x; y1 = y - 1;
    }
    else{
        x1 = x; y1 = y + 1;
    }
    check_pawn_move(moves, x1, y1, color, precalculation, 1);
    //2 squares forward
    if(color){
        x1 = x; y1 = y - 2;
    }
    else{
        x1 = x; y1 = y + 2;
    }
    check_pawn_move(moves, x1, y1, color, precalculation, 2);
    //Capture to the left
    if(color){
        x1 = x - 1; y1 = y - 1;
    }
    else{
        x1 = x + 1; y1 = y + 1;
    }
    check_pawn_move(moves, x1, y1, color, precalculation, 0);
    //Capture to the right
    if(color){
        x1 = x + 1; y1 = y - 1;
    }
    else{
        x1 = x - 1; y1 = y + 1;
    }
    check_pawn_move(moves, x1, y1, color, precalculation, 0);

    //End of precalculation
    if(precalculation){
        return;
    }

    //En passant
    if(color){
        x1 = x + 1;
        y1 = y - 1;
        check_en_passant(moves, x1, y1, color);
        x1 = x - 1;
        check_en_passant(moves, x1, y1, color);
    }
    else{
        x1 = x + 1;
        y1 = y + 1;
        check_en_passant(moves, x1, y1, color);
        x1 = x - 1;
        check_en_passant(moves, x1, y1, color);
    }
}

void BOARD::calculate_knight_moves(QVector<QPair<int, int>>& moves, int x, int y, bool color, bool precalculation){
    int x1, y1;
    //Left slightly up
    x1 = x - 2; y1 = y - 1;
    check_knight_move(moves, x1, y1, color, precalculation);
    //Left up
    x1 = x - 1; y1 = y - 2;
    check_knight_move(moves, x1, y1, color, precalculation);
    //Right up
    x1 = x + 1; y1 = y - 2;
    check_knight_move(moves, x1, y1, color, precalculation);
    //Right slightly up
    x1 = x + 2; y1 = y - 1;
    check_knight_move(moves, x1, y1, color, precalculation);
    //Right slightly down
    x1 = x + 2; y1 = y + 1;
    check_knight_move(moves, x1, y1, color, precalculation);
    //Right down
    x1 = x + 1; y1 = y + 2;
    check_knight_move(moves, x1, y1, color, precalculation);
    //Left down
    x1 = x - 1; y1 = y + 2;
    check_knight_move(moves, x1, y1, color, precalculation);
    //Left slightly down
    x1 = x - 2; y1 = y + 1;
    check_knight_move(moves, x1, y1, color, precalculation);
}

void BOARD::calculate_bishop_moves(QVector<QPair<int, int>>* moves, int x, int y, bool color, bool precalculation){
    int x1, y1;
    //Left Up
    x1 = x - 1; y1 = y - 1;
    while(check_long_range_move(moves, x1, y1, color, precalculation)){
        x1--; y1--;
    }
    //Left Down
    x1 = x - 1; y1 = y + 1;
    while(check_long_range_move(moves, x1, y1, color, precalculation)){
        x1--; y1++;
    }
    //Right Down
    x1 = x + 1; y1 = y + 1;
    while(check_long_range_move(moves, x1, y1, color, precalculation)){
        x1++; y1++;
    }
    //Right Up
    x1 = x + 1; y1 = y - 1;
    while(check_long_range_move(moves, x1, y1, color, precalculation)){
        x1++; y1--;
    }
}

void BOARD::calculate_rook_moves(QVector<QPair<int, int>>* moves, int x, int y, bool color, bool precalculation){
    int x1, y1;
    //Left
    x1 = x - 1; y1 = y;
    while(check_long_range_move(moves, x1, y1, color, precalculation)){
        x1--;
    }
    //Up
    x1 = x; y1 = y - 1;
    while(check_long_range_move(moves, x1, y1, color, precalculation)){
        y1--;
    }
    //Right
    x1 = x + 1; y1 = y;
    while(check_long_range_move(moves, x1, y1, color, precalculation)){
        x1++;
    }
    //Down
    x1 = x; y1 = y + 1;
    while(check_long_range_move(moves, x1, y1, color, precalculation)){
        y1++;
    }
}

void BOARD::calculate_queen_moves(QVector<QPair<int, int>>* moves, int x, int y, bool color, bool precalculation){
    int x1, y1;
    //Left
    x1 = x - 1; y1 = y;
    while(check_long_range_move(moves, x1, y1, color, precalculation)){
        x1--;
    }
    //Up
    x1 = x; y1 = y - 1;
    while(check_long_range_move(moves, x1, y1, color, precalculation)){
        y1--;
    }
    //Right
    x1 = x + 1; y1 = y;
    while(check_long_range_move(moves, x1, y1, color, precalculation)){
        x1++;
    }
    //Down
    x1 = x; y1 = y + 1;
    while(check_long_range_move(moves, x1, y1, color, precalculation)){
        y1++;
    }
    //Left Up
    x1 = x - 1; y1 = y - 1;
    while(check_long_range_move(moves, x1, y1, color, precalculation)){
        x1--; y1--;
    }
    //Left Down
    x1 = x - 1; y1 = y + 1;
    while(check_long_range_move(moves, x1, y1, color, precalculation)){
        x1--; y1++;
    }
    //Right Down
    x1 = x + 1; y1 = y + 1;
    while(check_long_range_move(moves, x1, y1, color, precalculation)){
        x1++; y1++;
    }
    //Right Up
    x1 = x + 1; y1 = y - 1;
    while(check_long_range_move(moves, x1, y1, color, precalculation)){
        x1++; y1--;
    }
}

void BOARD::calculate_king_moves(QVector<QPair<int, int>>& moves, int x, int y, bool color, bool precalculation){
    int x1, y1;
    //Left and Up
    x1 = x - 1; y1 = y - 1;
    check_king_move(moves, x1, y1, color, precalculation);
    //Up
    x1 = x; y1 = y - 1;
    check_king_move(moves, x1, y1, color, precalculation);
    //Right and Up
    x1 = x + 1; y1 = y - 1;
    check_king_move(moves, x1, y1, color, precalculation);
    //Right
    x1 = x + 1; y1 = y;
    check_king_move(moves, x1, y1, color, precalculation);
    //Right and Down
    x1 = x + 1; y1 = y + 1;
    check_king_move(moves, x1, y1, color, precalculation);
    //Down
    x1 = x; y1 = y + 1;
    check_king_move(moves, x1, y1, color, precalculation);
    //Left and Down
    x1 = x - 1; y1 = y + 1;
    check_king_move(moves, x1, y1, color, precalculation);
    //Left
    x1 = x - 1; y1 = y;
    check_king_move(moves, x1, y1, color, precalculation);
    //Castle
    if(!precalculation){
        x1 = x; y1 = y;
        check_castle(moves, x1, y1, color);
    }
}

void SQUARE::calculate_moves(bool precalculation, BOARD* cur_board){
    //Iterates over all the possible moves for current piece on given square
    int piece_type = piece->get_type();
    QVector<QPair<int, int>> moves;
    bool c = piece->is_white();
    if(piece_type == 1)
        cur_board->calculate_pawn_moves(moves, x, y, c, precalculation);
    else if(piece_type == 2)
        cur_board->calculate_knight_moves(moves, x, y, c, precalculation);
    else if(piece_type == 3)
        cur_board->calculate_bishop_moves(&moves, x, y, c, precalculation);
    else if(piece_type == 4)
        cur_board->calculate_rook_moves(&moves, x, y, c, precalculation);
    else if(piece_type == 5)
        cur_board->calculate_queen_moves(&moves, x, y, c, precalculation);
    else if(piece_type == 6)
        cur_board->calculate_king_moves(moves, x, y, c, precalculation);
    for(int i = 0; i < (int)moves.size(); ++i){
        precalculation ? cur_board->find_square(moves[i].first, moves[i].second)->set_protected(true) : possible_moves.push_back(cur_board->find_square(moves[i].first, moves[i].second));
    }
}

void SQUARE::highlight_possible_moves(){
    for(int i = 0; i < (int)possible_moves.size(); ++i){
        possible_moves[i]->set_possibly_highlighted(true);
    }
}

void SQUARE::unhighlight_possible_moves(){
    for(int i = 0; i < (int)possible_moves.size(); ++i){
        possible_moves[i]->set_possibly_highlighted(false);
    }
}

void SQUARE::clear_possible_moves(){
    possible_moves.clear();
}

BOARD::BOARD(){
    b.resize(8, QVector<SQUARE*> (8));
    sq_side = w / 8;
    turn = false;
    window_x = board_x;
    window_y = board_y;
    chosen_square = nullptr;
}

void BOARD::setup_board(std::string pos, bool proceed){
    //Making board filled with squares
    bool color_turn = true;
    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            b[i][j] = new SQUARE(color_turn, i, j, nullptr);
            color_turn = !color_turn;
        }
        color_turn = !color_turn;
    }
    //Setting up the board from string position representation (does NOT check if string may be converted into position correctly)
    SQUARE cur_pos = SQUARE(0, 0, 0, nullptr);
    std::string cur_int;
    for(int i = 0; i < (int)pos.length(); ++i){
        if(pos[i] >= '0' && pos[i] <= '9'){
            cur_int += pos[i];
            continue;
        }
        int current_number = 0;
        if(cur_int.length() > 0){
            current_number = std::stoi(cur_int);
            cur_pos.move_n_squares(current_number);
            cur_int.clear();
        }
        char piece;
        bool color;
        piece = pos[++i];
        if(pos[i - 1] == 'w'){
            color = true;
        }
        else{
            color = false;
        }
        //Creating a given piece on the board square
        b[cur_pos.X()][cur_pos.Y()]->assign_piece(convert_piece_type_to_int(piece), color);
        cur_pos.move_n_squares(1);
    }

    if(proceed)
        proceed_game(true, true, false);
}

void BOARD::print_board(QPainter *paint){
    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            if(b[i][j]->is_highlighted()){
                paint->fillRect(i*sq_side, j*sq_side, sq_side, sq_side, *Highlighted_Square);
            }
            else if(b[i][j]->is_possibly_highlighted()){
                paint->fillRect(i*sq_side, j*sq_side, sq_side, sq_side, *Possibly_Highlighted_Square);
                paint->drawRect(i*sq_side, j*sq_side, sq_side - 1, sq_side - 1);
            }
            else if(this->get_last_move().get_to() == b[i][j]){
                paint->fillRect(i*sq_side, j*sq_side, sq_side, sq_side, *Last_Move_Highlight);
            }
            else{
                paint->fillRect(i*sq_side, j*sq_side, sq_side, sq_side, (b[i][j]->is_white() ? *White_Square: *Black_Square));
            }

            if(b[i][j]->is_check_highlighted()){
                paint->fillRect(i*sq_side, j*sq_side, sq_side, sq_side, *Check_Highlighted_Square);
            }

            if(!(*b[i][j]).empty()){
                paint->drawImage(i*sq_side, j*sq_side, b[i][j]->get_piece()->get_piece_img()->scaled(sq_side, sq_side, Qt::KeepAspectRatio));
            }
        }
    }
}

std::string BOARD::position_to_string(){
    //Returns a string representation of current position on the board
    std::string pos;
    int cur_blank = 0;
    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            if(!b[j][i]->empty()){
                if(cur_blank > 0){
                    pos += std::to_string(cur_blank);
                    cur_blank = 0;
                }
                pos += (b[j][i]->get_piece()->is_white() ? 'w' : 'b');
                pos += b[j][i]->get_piece()->get_char_type();
            }
            else{
                cur_blank++;
            }
        }
    }
    if(cur_blank > 0){
        pos += std::to_string(cur_blank);
    }
    return pos;
}

bool BOARD::on_main_board(int x, int y){
    //Checks if mouse click was on main board
    int win_x = win.x();
    int win_y = win.y();
    if(x < window_x + win_x || x > window_x + 8*sq_side + win_x || y < window_y + win_y || y > window_y + 8*sq_side + win_y)
        return false;
    return true;
}

int MOVE::get_move_type(BOARD* cur_board){
    if(move_type == -100){
        calculate_move_type(cur_board);
    }
    return move_type;
}

void MOVE::calculate_move_type(BOARD* cur_board){
    //-1 - capture move
    //0 - regular move
    //1 - king side castle
    //2 - queen side castle
    //3 - en passant
    //4 - pawn promotion
    if(from == nullptr){
        //exception
        return;
    }
    int i = to->X();
    int j = to->Y();
    bool turn = cur_board->get_turn();
    QVector<QVector<SQUARE*>> b = cur_board->get_full_board();
    if(multiple_check_if_index_on_board({{i, j + 1}, {i, j - 1}}) && b[i][j]->empty() && from->get_piece()->get_type() == 1 && !(turn ? b[i][j + 1] : b[i][j - 1])->empty() && (turn ? b[i][j + 1] : b[i][j - 1])->get_piece()->get_type() == 1 && (turn ? b[i][j + 1] : b[i][j - 1])->get_piece()->is_white() != turn){
        //Means en passant was played
        move_type = 3;
        return;
    }
    else if(!b[i][j]->empty() && b[i][j]->get_piece()->is_white() == from->get_piece()->is_white() && from->get_piece()->get_type() == 6 && to->get_piece()->get_type() == 4){
        //Means castle move was played
        move_type = (i == 0) ? 2 : 1;
        return;
    }
    else if(from->get_piece()->get_type() == 1 && to->Y() == (turn ? 0 : 7)){
        //Promotion
        move_type = 4;
        return;
    }
    else if(!to->empty() && to->get_piece()->is_white() != turn){
        //Capture
        move_type = -1;
    }
    move_type = 0;
}

void BOARD::move_piece(int i, int j){
    //Moves piece from chosen square to square[i][i] assuming move is legal and changes the board
    MOVE move(chosen_square, b[i][j], this);
    int x = move.get_from()->X();
    int y = move.get_from()->Y();
    int move_type = move.get_move_type(this);
    if(move_type == 1){ //King side castle
        b[i - 1][j]->assign_piece(b[x][y]->get_piece());
        b[x][y]->get_piece()->mark_moved();
        b[i - 2][j]->assign_piece(b[i][j]->get_piece());
        b[i - 2][j]->get_piece()->mark_moved();
        b[i][j]->set_empty();
    }
    else if(move_type == 2){ //Queen side castle
        b[i + 2][j]->assign_piece(b[x][y]->get_piece());
        b[x][y]->get_piece()->mark_moved();
        b[i + 3][j]->assign_piece(b[i][j]->get_piece());
        b[i + 3][j]->get_piece()->mark_moved();
        b[i][j]->set_empty();
    }
    else if(move_type == 3){ //En passant
        b[i][j]->assign_piece(b[x][y]->get_piece());
        b[x][y]->get_piece()->has_moved();
        (turn ? b[i][j + 1] : b[i][j - 1])->set_empty();
    }
    else if(move_type == 4){ //Promotion move
        PIECE new_piece = PIECE(5, turn);
        //delete b[i][j]->get_piece(); - memory leak here
        b[i][j]->assign_copy_of_piece(&new_piece);
        /*
        if(AI_used){
            //Promote to a queen
        }
        else{
            //Give player a chanse to pick piece (not done yet)
            //int chosen_type_of_piece = process
        }
        */
    }
    else{ //Regular move
        b[i][j]->assign_piece(b[x][y]->get_piece());
        b[x][y]->get_piece()->mark_moved();
    }
    add_move(b[x][y], b[i][j]);
    b[x][y]->set_empty();
    //release_chosen_square();

    /*
    int move_type = move.get_move_type(this);

    if(!b[i][j]->empty() && b[i][j]->get_piece()->get_type() == 6){
        int debug = 0;
    }

    if(move_type == 1){ //King side castle
        b[i + 1][j]->assign_piece(chosen_square->get_piece());
        chosen_square->get_piece()->mark_moved();
        b[i + 2][j]->assign_piece(b[i][j]->get_piece());
        b[i + 2][j]->get_piece()->mark_moved();
        b[i][j]->set_empty();
    }
    else if(move_type == 2){ //Queen side castle
        b[i - 2][j]->assign_piece(chosen_square->get_piece());
        b[i - 3][j]->assign_piece(b[i][j]->get_piece());
        b[i - 3][j]->get_piece()->mark_moved();
        b[i][j]->set_empty();
    }
    else if(move_type == 3){ //En passant
        b[i][j]->assign_piece(chosen_square->get_piece());
        //chosen_square->get_piece()->mark_moved();
        (turn ? b[i][j + 1] : b[i][j - 1])->set_empty();
    }
    else if(move_type == 4){ //Promotion move
        PIECE new_piece = PIECE(5, turn);
        //delete b[i][j]->get_piece(); - memory leak here
        b[i][j]->assign_copy_of_piece(&new_piece);
        if(AI_used){
            //Promote to a queen
        }
        else{
            //Give player a chanse to pick piece (not done yet)
            //int chosen_type_of_piece = process
        }
    }
    else{ //Regular move
        b[i][j]->assign_piece(chosen_square->get_piece());
    }
    chosen_square->get_piece()->mark_moved();
    add_move(chosen_square, b[i][j]);
    chosen_square->set_empty();

    //Prints position after move happened in debug stream
    qDebug() << "\n";
    for(int i = 0; i < 8; ++i){
        QString cur;
        for(int j = 0; j < 8; ++j){
            cur += (QChar)((b[i][j]->empty() ? 0 : b[i][j]->get_piece()->get_type()));
        }
        qDebug() << cur;
    }

    release_chosen_square();
    */
}

void BOARD::move_piece(MOVE move){
    //Moves piece from chosen square to square[i][i] assuming move is legal and changes the board;
    int i = move.get_to()->X();
    int j = move.get_to()->Y();
    int x = move.get_from()->X();
    int y = move.get_from()->Y();
    int move_type = move.get_move_type(this);

    if(move_type == 1){ //King side castle
        b[i - 1][j]->assign_piece(b[x][y]->get_piece());
        b[x][y]->get_piece()->mark_moved();
        b[i - 2][j]->assign_piece(b[i][j]->get_piece());
        b[i - 2][j]->get_piece()->mark_moved();
        b[i][j]->set_empty();
    }
    else if(move_type == 2){ //Queen side castle
        b[i + 2][j]->assign_piece(b[x][y]->get_piece());
        b[x][y]->get_piece()->mark_moved();
        b[i + 3][j]->assign_piece(b[i][j]->get_piece());
        b[i + 3][j]->get_piece()->mark_moved();
        b[i][j]->set_empty();
    }
    else if(move_type == 3){ //En passant
        b[i][j]->assign_piece(b[x][y]->get_piece());
        b[x][y]->get_piece()->has_moved();
        (turn ? b[i][j + 1] : b[i][j - 1])->set_empty();
    }
    else if(move_type == 4){ //Promotion move
        PIECE new_piece = PIECE(5, turn);
        //delete b[i][j]->get_piece(); - memory leak here
        b[i][j]->assign_copy_of_piece(&new_piece);
        /*
        if(AI_used){
            //Promote to a queen
        }
        else{
            //Give player a chanse to pick piece (not done yet)
            //int chosen_type_of_piece = process
        }
        */
    }
    else{ //Regular move
        b[i][j]->assign_piece(b[x][y]->get_piece());
        b[x][y]->get_piece()->mark_moved();
    }
    add_move(b[x][y], b[i][j]);
    b[x][y]->set_empty();
    //release_chosen_square();

    /*
    //chosen_square->set_empty();    
    //Prints position after move happened in debug stream
    qDebug() << "\n";
    for(int i = 0; i < 8; ++i){
        QString cur;
        for(int j = 0; j < 8; ++j){
            cur += (QChar)((b[i][j]->empty() ? 0 : b[i][j]->get_piece()->get_type()));
        }
        qDebug() << cur;
    }
    */

    //release_chosen_square();
}

void BOARD::set_square_as_chosen(int i, int j){
    if(b[i][j]->empty())
        return;
    chosen_square = b[i][j];
    chosen_square->set_highlighted(true);
    chosen_square->highlight_possible_moves();
}

void BOARD::release_chosen_square(){
    chosen_square->unhighlight_possible_moves();
    chosen_square->set_highlighted(false);
    chosen_square = nullptr;
}

bool BOARD::check_if_can_be_chosen(int x, int y){
    //Checks if square[x][y] could be set as chosen considering current turn
    if(b[x][y]->empty()){
        return false;
    }
    bool piece_color = b[x][y]->get_piece()->is_white();
    if(piece_color != turn){
        return false;
    }
    return true;
}

int BOARD::mouse_click(int x, int y){
    //Processing mouse click event
    const int title_bar_y = 25;
    int win_x = win.x();
    int win_y = win.y();
    int i = (x - window_x - win_x) / sq_side;
    int j = (y - window_y - win_y - title_bar_y) / sq_side;
    if(is_any_square_chosen()){
        if(chosen_square->check_if_move_is_possible(i, j)){
            move_piece(i, j);
            release_chosen_square();
            return 1;
        }
        else{
            release_chosen_square();
            return 0;
        }
    }
    //else (no square is chosen)
    if(check_if_can_be_chosen(i, j)){
        set_square_as_chosen(i, j);
        return 0;
    }
    return 0;
}

bool index_on_board(int i, int j){
    //Checks if given pair {i, j} could be the square on the board
    if(i < 0 || i > 7 || j < 0 || j > 7)
        return false;
    return true;
}

bool multiple_check_if_index_on_board(QVector<QPair<int, int>> idx){
    for(int i = 0; i < (int)idx.size(); ++i){
        if(!index_on_board(idx[i].first, idx[i].second)){
            return false;
        }
    }
    return true;
}

void BOARD::extand_protected_recursively(){
    //Makes all the squares that are currently under cover of enemy pieces flag "protected"

    //Iteration over all the possible enemy moves
    QVector<SQUARE*> protected_stack;
    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            if(!b[i][j]->empty() && b[i][j]->get_piece()->is_white() != turn){
                protected_stack.push_back(b[i][j]);
            }
        }
    }

    //Setting flags for all the found moves
    for(int i = 0; i < (int)protected_stack.size(); ++i){
        QVector<SQUARE*> cur_possible_moves = protected_stack[i]->get_possible_moves();
        for(int j = 0; j < (int)cur_possible_moves.size(); ++j){
            cur_possible_moves[j]->set_protected(true);
        }
    }

    //Getting rid of possible moves that would get king into a check (no need for now)
    /*
    QPair<int, int> pos = find_king_coords(turn);
    QVector<SQUARE*> king_possible_moves = b[pos.first][pos.second]->get_possible_moves();
    for(int i = 0; i < (int)king_possible_moves.size(); ++i){
        if(king_possible_moves[i]->is_protected()){
            b[pos.first][pos.second]->erase_from_possible_moves(i, i + 1);
            king_possible_moves.erase(king_possible_moves.begin() + i, king_possible_moves.begin() + i + 1);
            --i;
        }
    }
    */
}

void BOARD::protection_reset(){
    //Resets flag "protected" from all squares on the board
    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            b[i][j]->set_protected(false);
        }
    }
}

void BOARD::precalculate_protected(){
    //Calculates all the moves for enemy pieces and then sets every covered (attacked) square as "protected"
    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            if(!b[i][j]->empty() && b[i][j]->get_piece()->is_white() != turn){
                b[i][j]->calculate_moves(true, this);
                QVector<SQUARE*> possible_moves = b[i][j]->get_possible_moves();
                for(int u = 0; u < (int)possible_moves.size(); ++u){
                    if(possible_moves[u]->empty() || (!possible_moves[u]->empty() && possible_moves[u]->get_piece()->is_white() != turn)){
                        possible_moves[u]->set_protected(true);
                    }
                }
            }
        }
    }
}

void BOARD::calculate_possible_moves(){
    //Calculates all the possible moves for pieces that are same color as turn
    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            if(!b[i][j]->empty()){
                b[i][j]->calculate_moves(false, this);
            }
        }
    }
}

void BOARD::remove_moves_not_preventing_check(){
    //Removes moves that are stored in possible_moves but actually illegal because they lead to the positions where your own king could be captured
    QVector<MOVE> cur_possible_moves = this->get_all_possible_moves(turn);
    if(cur_possible_moves.size() == 0) return;
    SQUARE* cur_sq = cur_possible_moves[0].get_from();
    int z = -1;
    for(int u = 0; u < (int)cur_possible_moves.size(); ++u){
        MOVE cur_move = cur_possible_moves[u];
        if(cur_move.get_from() != cur_sq){
            z = 0;
            cur_sq = cur_move.get_from();
        }
        else{
            z++;
        }
        int i = cur_move.get_from()->X();
        int j = cur_move.get_from()->Y();
        if(is_check(true).first && (cur_move.get_move_type(this) == 1 || cur_move.get_move_type(this) == 2)){
            //Castle while own king is in check
            b[i][j]->erase_from_possible_moves(z, z + 1);
            cur_possible_moves.erase(cur_possible_moves.begin() + u, cur_possible_moves.begin() + u + 1);
            --u;
            --z;
            continue;
        }
        BOARD* transformed_pos = transform_to_next_position(this, cur_move, false, true);
        QPair<bool, SQUARE*> transformed_pos_check = transformed_pos->is_check(true);
        if(transformed_pos_check.first && transformed_pos_check.second->get_piece()->is_white() == turn){
            b[i][j]->erase_from_possible_moves(z, z + 1);
            cur_possible_moves.erase(cur_possible_moves.begin() + u, cur_possible_moves.begin() + u + 1);
            --u;
            --z;
        }
        delete transformed_pos;
    }
}

void BOARD::process_checks(){
    //Checks if there is a check on the board and if so makes changes on the board
    QPair<bool, SQUARE*> possible_check_pos = is_check(true);
    if(possible_check_pos.first){
        set_check_on_board(possible_check_pos);
        possible_check_pos.second->set_check_hightighted(true);
    }
    else{
        if(get_current_check_pair().second != nullptr){
            get_current_check_pair().second->set_check_hightighted(false);
        }
        set_check_on_board({false, nullptr});
    }
    //remove_moves_not_preventing_check();
}

int BOARD::proceed_game(bool check_find, bool switch_turn, bool one_step){
    //Makes changes and recalculation on the board after move happened
    //Returns -1 or 1 if black or white has won, 0 if stalemate on the board, 100 if the game must go on
    if(switch_turn){
        turn = !turn;
    }
    clear_possible_moves();
    protection_reset();
    precalculate_protected();
    calculate_possible_moves();
    //extand_protected_recursively();
    if(!one_step){
        remove_moves_not_preventing_check();
    }
    if(check_find && !one_step){
        process_checks();
        if(is_checkmate(false)){
            //checkmate
            return turn ? -1 : 1;
        }
        if(is_stalemate()){
            //stalemate
            return 0;
        }
    }
    if(this == board){
        D_PrintPosition(this);
    }
    return 100;
}

SQUARE* BOARD::find_square(int i, int j){
    //returns pointer to square assuming it is on the board
    return b[i][j];
}

SQUARE* BOARD::find_square(QPair<int, int> pos){
    //returns pointer to square assuming it is on the board
    return b[pos.first][pos.second];
}

void BOARD::clear_possible_moves(){
    //Clears arrays of possible_moves for each square on the board
    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            b[i][j]->clear_possible_moves();
        }
    }
}

QPair<bool, SQUARE*> BOARD::is_check(bool reiterate){
    //Checks if check is on the board
    //If reiterate == true makes complete reiteration through opponent moves to check if they are pointing to own king, else returns memorized flag
    if(!reiterate){
        return check_on_board;
    }
    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            if(!b[i][j]->empty() && b[i][j]->get_piece()->is_white() != turn){
                QVector<SQUARE*> cur_opponent_moves = b[i][j]->get_possible_moves();
                QPair<int, int> cur_king_pos = find_king_coords(turn);
                for(int u = 0; u < (int)cur_opponent_moves.size(); ++u){
                    if(cur_opponent_moves[u]->X() == cur_king_pos.first && cur_opponent_moves[u]->Y() == cur_king_pos.second){
                        //Check found
                        return {true, b[cur_king_pos.first][cur_king_pos.second]};
                    }
                }
            }
        }
    }
    //No check
    return {false, nullptr};
}

QPair<int, int> BOARD::find_king_coords(bool color){
    //Returns coords of given color king's current position on the board
    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            if(!b[i][j]->empty() && b[i][j]->get_piece()->get_type() == 6 && b[i][j]->get_piece()->is_white() == color){
                return {i, j};
            }
        }
    }
    //Awful exception :)
    return {-1, -1};
}

bool BOARD::is_checkmate(bool reiterate_inside_is_check_func){
    //Checks if checkmate is on the board
    if(!is_check(reiterate_inside_is_check_func).first){
        return false;
    }
    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            if(!b[i][j]->empty() && b[i][j]->get_piece()->is_white() == turn && !b[i][j]->get_possible_moves().empty()){
                return false;
            }
        }
    }
    return true;
}

bool BOARD::is_stalemate(){
    //Checks if stalemate is on the board
    if(is_check(false).first){
        return false;
    }
    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            if(!b[i][j]->empty() && b[i][j]->get_piece()->is_white() == turn && (int)b[i][j]->get_possible_moves().size() > 0){
                return false;
            }
        }
    }
    return true;
}

void BOARD::add_move(SQUARE *from, SQUARE *to){
    if(from == nullptr || to == nullptr){
        return;
    }
    //Adds move to the back of list of played moves
    played_moves.push_back(MOVE(from, to, this));
}

void BOARD::add_move(QPair<SQUARE *, SQUARE *> move_to_add){
    if(move_to_add.first == nullptr || move_to_add.second == nullptr){
        return;
    }
    //Adds move to the back of list of played moves
    played_moves.push_back(MOVE(move_to_add.first, move_to_add.second, this));
}

MOVE BOARD::get_last_move(){
    //Returns last move that happened in the game, or {0, 0} if there weren't any
    if((int)played_moves.size() > 0){
        return played_moves.back();
    }
    //exception (no moves were played)
    return MOVE(nullptr, nullptr, nullptr);
}

BOARD* BOARD::copy_board(BOARD* from){
    //Creates a new board which is complete copy of given and returns a pointer to the new board
    BOARD* board_ptr = new BOARD;
    std::string cur_pos = from->position_to_string();
    board_ptr->setup_board(cur_pos, false);
    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            if(!board_ptr->b[i][j]->empty() && from->b[i][j]->get_piece()->has_moved()){
                board_ptr->b[i][j]->get_piece()->mark_moved();
            }
        }
    }
    if(from->turn){
        board_ptr->turn = !board_ptr->turn;
    }
    board_ptr->assign_played_moves_order(from->get_all_played_moves());
    return board_ptr;
}

void BOARD::assign_played_moves_order(QVector<MOVE> moves){
    played_moves.clear();
    played_moves.resize((int)moves.size());
    played_moves = moves;
}

QVector<MOVE> BOARD::get_all_played_moves(){
    return played_moves;
}

BOARD* BOARD::transform_to_next_position(BOARD* prev_board, MOVE move, bool switch_turn, bool one_step){
    //Creates a new board where given move was played and returns a pointer to it
    //NOTE: this function DOES NOT switch turn on the new board
    BOARD* new_board_ptr = copy_board(prev_board);
    if(switch_turn){
        new_board_ptr->switch_turn();
    }
    new_board_ptr->set_square_as_chosen(move.first()->X(), move.first()->Y());
    new_board_ptr->move_piece(move.second()->X(), move.second()->Y());
    new_board_ptr->proceed_game(false, false, one_step);
    return new_board_ptr;
}

float BOARD::coef_for_dist_to_opponent_king(int i, int j, int piece_type, bool king_col){
    QPair<int, int> king_coords = this->find_king_coords(king_col);
    int x = king_coords.first;
    int y = king_coords.second;
    float dist = std::max(abs(i - x), abs(j - y));
    float bonus = 1.f + ((4.f - dist) / 8.f);
    if(piece_type == 4 || piece_type == 5 || piece_type == 6){
        bonus += (bonus > 0.f ? 0.1f : -0.1f);
    }
    else if(piece_type == 3 || piece_type == 1){
         bonus += (bonus > 0.f ? 0.05f : -0.05f);
    }
    return bonus;
}

float BOARD::get_evaluation(){
    if(is_checkmate(true)){
        //Checkmate on the board
        return -INF;
    }
    else if(is_stalemate()){
        //Stalemate on the board
        return 0;
    }
    float white_eval = 0;
    float black_eval = 0;
    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            if(!b[i][j]->empty()){
                int piece_type = b[i][j]->get_piece()->get_type();
                if(b[i][j]->get_piece()->is_white()){
                    white_eval += ((float)b[i][j]->get_piece()->get_evaluation_value())*get_coeff(i, j, piece_type)*coef_for_dist_to_opponent_king(i, j, piece_type, false);
                }
                else{ //black
                    black_eval += ((float)b[i][j]->get_piece()->get_evaluation_value())*get_coeff(i, j, piece_type)*coef_for_dist_to_opponent_king(i, j, piece_type, true);
                }
            }
        }
    }
    float eval = white_eval - black_eval;
    return turn ? eval : -eval;
}

QVector<MOVE> BOARD::get_all_possible_moves(bool color){
    QVector<MOVE> moves;
    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            if(!b[i][j]->empty() && b[i][j]->get_piece()->is_white() == color){
                QVector<SQUARE*> cur_moves = b[i][j]->get_possible_moves();
                for(int u = 0; u < (int)cur_moves.size(); ++u){
                    moves.push_back(MOVE(b[i][j], cur_moves[u], this));
                }
            }
        }
    }
    return moves;
}


void BOARD::make_AI_move(){
    AI_MAIN_TREE->start_tree_from_new_node();
    AI_MAIN_TREE->recalculate_tree(DEPTH);
    MOVE best_move = AI_MAIN_TREE->get_next_move();
    move_piece(best_move);
}

void BOARD::make_BETTER_AI_move(){
    MOVE best_move = search(this);
    move_piece(best_move);
}

BOARD::~BOARD(){
    for(int i = 0; i < b.size(); ++i){
        for(int j = 0; j < b[i].size(); ++j){
            delete b[i][j];
        }
    }
}

void load_from_file(int piece_type, QVector<QVector<float>>& storage){
    std::string filename;
    storage.resize(8, QVector<float> (8));
    if(piece_type == 1){
        filename = filename = "./Piece_coef/pawn.txt";
    }
    else if(piece_type == 2){
        filename = "./Piece_coef/knight.txt";
        //filename = ":/Coefs/Piece_coef/bishop.txt";
    }
    else if(piece_type == 3){
        filename = "./Piece_coef/bishop.txt";
        //filename = ":/Coefs/Piece_coef/bishop.txt";
    }
    else if(piece_type == 4){
        filename = "./Piece_coef/rook.txt";
        //filename = ":/Coefs/Piece_coef/rook.txt";
    }
    else if(piece_type == 5){
        filename = "./Piece_coef/queen.txt";
        //filename = ":/Coefs/Piece_coef/queen.txt";
    }
    else if(piece_type == 6){
        filename = "./Piece_coef/king.txt";
        //filename = ":/Coefs/Piece_coef/king.txt";
    }
    else{
        //?
        return;
    }
    std::ifstream file(filename);
    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            float cur;
            file >> cur;
            //cur = abs(1 - cur) / 3.5f;
            float dif = (cur - 1) / 5.f;
            storage[i][j] = dif + 1.0f;
        }
    }
    file.close();
}

float get_coeff(int i, int j, int piece_type){
    if(piece_type == 1){ //Pawn
        return c_p[i][j];
    }
    else if(piece_type == 2){ //Knight
        return c_kn[i][j];
    }
    else if(piece_type == 3){
        return c_b[i][j];
    }
    else if(piece_type == 4){
        return c_r[i][j];
    }
    else if(piece_type == 5){
        return c_q[i][j];
    }
    else if(piece_type == 6){
        return c_king[i][j];
    }
    return 1.0f;
}

bool BOARD::BETTER_is_check(){
    SQUARE* king_pos = find_square(find_king_coords(turn));
    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            if(!b[i][j]->empty() && b[i][j]->get_piece()->is_white() != turn){
                QVector<SQUARE*> all_moves = b[i][j]->get_possible_moves();
                for(int z = 0; z < (int)all_moves.size(); ++z){
                    if(king_pos == all_moves[z]){
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

int BOARD::BETTER_is_checkmate(){
    QPair<int, int> king_pos_pair = find_king_coords(turn);
    if(king_pos_pair.first == -1){
        //King got captured
        return 1;
    }
    if(!BETTER_is_check()){
        return 0;
    }
    SQUARE* king_pos = find_square(king_pos_pair);
    QVector<SQUARE*> king_moves = king_pos->get_possible_moves();
    if(king_moves.empty()){
        return 1;
    }
    return 0;
}

int BOARD::BETTER_game_status(){
    if(BETTER_is_checkmate()){
        return -1;
    }
    if(BETTER_is_draw()){
        return 0;
    }
    return 1;
}

bool BOARD::BETTER_is_stalemate(){
    if(BETTER_is_check()){
        return false;
    }
    if(get_all_possible_moves(turn).empty()){
        return true;
    }
    return false;
}

bool BOARD::BETTER_is_draw_by_repetition(){
    QVector<MOVE> all_played_moves = get_all_played_moves();
    int total = all_played_moves.size();
    if(total < 10){
        return false;
    }
    //Needs to be done
    return false; //This statement is incorrect
}

bool BOARD::BETTER_is_draw(){
    if(BETTER_is_stalemate() || BETTER_is_draw_by_repetition()){
        return true;
    }
    return false;
}

float BOARD::BETTER_EVALUATE_POSITION(){
    float white = 0;
    float black = 0;
    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            if(!b[i][j]->empty()){
                PIECE* cur_piece = b[i][j]->get_piece();
                int piece_val = cur_piece->get_evaluation_value()*get_coeff(i, j, cur_piece->get_type());
                b[i][j]->get_piece()->is_white() ? white += piece_val : black += piece_val;
            }
        }
    }
    return white - black;
}

float BOARD::BETTER_EVALUATION(){
    int status = BETTER_game_status();
    switch(status){
        case -1: return -FINF;
        case 0: return 0.f;
    }
    int unsign_eval = BETTER_EVALUATE_POSITION();
    return turn ? unsign_eval : -unsign_eval;
}
