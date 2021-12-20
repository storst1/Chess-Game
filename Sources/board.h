#ifndef BOARD_H
#define BOARD_H
#include <mainwindow.h>

class SQUARE;
class BOARD;
class PAWN;
class KNIGHT;
class BISHOP;
class ROOK;
class QUEEN;
class KING;

extern int w;

void create_board_objects();
bool multiple_check_if_index_on_board(QVector<QPair<int, int>> idx);
bool index_on_board(int i, int j);
void load_from_file(int piece_type, QVector<QVector<float>>& storage);
float get_coeff(int i, int j, int piece_type);

class PIECE{
private:
    int type;
    bool color;
    QImage* Img;
    bool moved;
public:
    PIECE(int type, bool color);
    QImage* get_piece_img();
    bool is_white(){ return color; };
    int get_type();
    char get_char_type();
    bool has_moved(){ return moved; };
    void mark_moved(){ moved = true; };
    int get_evaluation_value();
    ~PIECE();
};

class SQUARE{
private:
    bool color;
    int x;
    int y;
    PIECE* piece;
    bool highlighted;
    bool possible_highlighted;
    QVector<SQUARE*> possible_moves;
    bool Protected;
    bool check_highlighted;
public:
    SQUARE():highlighted(false), possible_highlighted(false), Protected(false){};
    SQUARE(bool color, int x, int y, PIECE* piece):color(color), x(x), y(y), piece(piece), highlighted(false), possible_highlighted(false), Protected(false), check_highlighted(false){};
    SQUARE(bool color, QPair<int, int> xy, PIECE* piece):color(color), x(xy.first), y(xy.second), piece(piece), highlighted(false), possible_highlighted(false), Protected(false), check_highlighted(false){};
    int X(){return x;};
    int Y(){return y;};
    QPair<int, int> pos(){ return {x, y}; };
    bool on_the_board();
    bool empty();
    void move_n_squares(int n);
    void assign_piece(int type, bool color);
    void assign_piece(PIECE* _piece);
    void assign_copy_of_piece(PIECE* _piece);
    bool is_white();
    PIECE* get_piece();
    void set_color(bool new_color){color = new_color; };
    void set_empty();
    bool is_highlighted(){ return highlighted; };
    bool is_possibly_highlighted(){ return possible_highlighted; };
    bool is_check_highlighted(){ return check_highlighted; };
    void set_highlighted(bool value){ highlighted = value; };
    void set_possibly_highlighted(bool value){ possible_highlighted = value; };
    void set_check_hightighted(bool value){ check_highlighted = value; };
    void calculate_moves(bool precalculation, BOARD* cur_board);
    void clear_possible_moves();
    bool is_protected(){ return Protected; };
    void set_protected(bool value){ Protected = value; };
    QVector<SQUARE*> get_possible_moves(){ return possible_moves; };
    void highlight_possible_moves();
    void unhighlight_possible_moves();
    bool check_if_move_is_possible(int i, int j);
    void erase_from_possible_moves(int from, int to);
    ~SQUARE();
};

class MOVE{
private:
    SQUARE* from;
    SQUARE* to;
    BOARD* _board;
    PIECE* piece;
    int move_type = -100;
    int promoted_to_piece = -1;
public:
    MOVE(){};
    MOVE(SQUARE* from, SQUARE* to, BOARD* cur_board);
    SQUARE* get_from(){ return from; };
    SQUARE* get_to(){ return to; };
    BOARD* get_board(){ return _board; };
    PIECE* get_piece(){ return piece; };
    void calculate_move_type(BOARD* cur_board);
    int get_move_type(BOARD* cur_board);
    SQUARE* first(){ return from; };
    SQUARE* second(){ return to; };
    ~MOVE();
};

class PAWN{
private:
    QImage* ImgBlack = new QImage("./Images/Black_Pawn2.png");
    QImage* ImgWhite = new QImage("./Images/White_Pawn2.png");
public:
    PAWN(){};
    QImage* BLACK(){return ImgBlack;};
    QImage* WHITE(){return ImgWhite;};
};

class KNIGHT{
private:
    QImage* ImgBlack = new QImage("./Images/Black_Knight2.png");
    QImage* ImgWhite = new QImage("./Images/White_Knight2.png");
public:
    KNIGHT(){};
    QImage* BLACK(){return ImgBlack;};
    QImage* WHITE(){return ImgWhite;};
};

class BISHOP{
private:
    QImage* ImgBlack = new QImage("./Images/Black_Bishop2.png");
    QImage* ImgWhite = new QImage("./Images/White_Bishop2.png");
public:
    BISHOP(){};
    QImage* BLACK(){return ImgBlack;};
    QImage* WHITE(){return ImgWhite;};
};

class ROOK{
private:
    QImage* ImgBlack = new QImage("./Images/Black_Rook2.png");
    QImage* ImgWhite = new QImage("./Images/White_Rook2.png");
public:
    ROOK(){};
    QImage* BLACK(){return ImgBlack;};
    QImage* WHITE(){return ImgWhite;};
};

class QUEEN{
private:
    QImage* ImgBlack = new QImage("./Images/Black_Queen2.png");
    QImage* ImgWhite = new QImage("./Images/White_Queen2.png");
public:
    QUEEN(){};
    QImage* BLACK(){return ImgBlack;};
    QImage* WHITE(){return ImgWhite;};
};

class KING{
private:
    QImage* ImgBlack = new QImage("./Images/Black_King2.png");
    QImage* ImgWhite = new QImage("./Images/White_King2.png");
public:
    KING(){};
    QImage* BLACK(){return ImgBlack;};
    QImage* WHITE(){return ImgWhite;};
};

class BOARD{
private:
    QVector<QVector<SQUARE*>> b;
    int sq_side;
    bool turn;
    int window_x;
    int window_y;
    SQUARE* chosen_square;
    QPair<bool, SQUARE*> check_on_board;
    QVector<MOVE> played_moves;
public:
    BOARD();
    void setup_board(std::string pos, bool proceed);
    void print_board(QPainter *paint);
    int get_square_size(){ return sq_side; };
    SQUARE* get_square(int i, int j){ return b[i][j]; };
    std::string position_to_string();
    bool get_turn(){ return turn; };
    void switch_turn(){ turn = !turn; };
    bool on_main_board(int x, int y);
    bool is_any_square_chosen(){ return (chosen_square == nullptr) ? false : true; };
    int mouse_click(int x, int y);
    void move_piece(int i, int j);
    void move_piece(MOVE move);
    void set_square_as_chosen(int i, int j);
    int proceed_game(bool check_find, bool switch_turn, bool one_step);
    //bool index_on_board(int i, int j);
    void precalculate_protected();
    void calculate_possible_moves();
    void clear_possible_moves();
    SQUARE* find_square(int i, int j);
    SQUARE* find_square(QPair<int, int> pos);
    void release_chosen_square();
    bool check_if_can_be_chosen(int x, int y);
    void protection_reset();
    QPair<bool, SQUARE*> is_check(bool reiterate);
    QPair<bool, SQUARE*> get_current_check_pair(){ return check_on_board; };
    void set_check_on_board(QPair<bool, SQUARE*> check_pair){ check_on_board = check_pair; };
    QPair<int, int> find_king_coords(bool color);
    void process_checks();
    void extand_protected_recursively();
    void remove_moves_not_preventing_check();
    BOARD* transform_to_next_position(BOARD* prev_board, MOVE move, bool switch_turn, bool one_step);
    BOARD* copy_board(BOARD* from);
    void calculate_pawn_moves(QVector<QPair<int, int>>& moves, int x, int y, bool color, bool precalculation);
    void calculate_king_moves(QVector<QPair<int, int>>& moves, int x, int y, bool color, bool precalculation);
    void calculate_knight_moves(QVector<QPair<int, int>>& moves, int x, int y, bool color, bool precalculation);
    void calculate_rook_moves(QVector<QPair<int, int>>* moves, int x, int y, bool color, bool precalculation);
    void calculate_bishop_moves(QVector<QPair<int, int>>* moves, int x, int y, bool color, bool precalculation);
    void calculate_queen_moves(QVector<QPair<int, int>>* moves, int x, int y, bool color, bool precalculation);
    void check_king_move(QVector<QPair<int, int>>& moves, int x1, int y1, bool color, bool precalculation);
    void check_pawn_move(QVector<QPair<int, int>>& moves, int x1, int y1, bool color, bool precalculation, int forward);
    bool check_long_range_move(QVector<QPair<int, int>>* moves, int x1, int y1, bool color, bool precalculation);
    void check_knight_move(QVector<QPair<int, int>>& moves, int x1, int y1, bool color, bool precalculation);
    void check_castle(QVector<QPair<int, int>>& moves, int x1, int y1, bool color);
    void check_en_passant(QVector<QPair<int, int>>& moves, int x1, int y1, bool color);
    bool is_checkmate(bool reiterate_inside_is_check_func);
    bool is_stalemate();
    void add_move(SQUARE* from, SQUARE* to);
    void add_move(QPair<SQUARE*, SQUARE*> move_to_add);
    void add_move(MOVE move_to_add){ played_moves.push_back(move_to_add); };
    MOVE get_last_move();
    int get_move_type(SQUARE* from, SQUARE* to);
    float get_evaluation();
    QVector<MOVE> get_all_possible_moves(bool color);
    QVector<QVector<SQUARE*>> get_full_board(){ return b; };
    void make_AI_move();
    QVector<MOVE> get_all_played_moves();
    void assign_played_moves_order(QVector<MOVE> moves);
    float coef_for_dist_to_opponent_king(int i, int j, int piece_type, bool king_col);
    float BETTER_EVALUATION();
    int BETTER_is_checkmate();
    bool BETTER_is_check();
    int BETTER_game_status();
    bool BETTER_is_draw();
    bool BETTER_is_stalemate();
    bool BETTER_is_draw_by_repetition();
    float BETTER_EVALUATE_POSITION();
    void make_BETTER_AI_move();
    ~BOARD();
};

#endif // BOARD_H
