#ifndef AI_H
#define AI_H

#include <board.h>
//C++ includes
#include <chrono>
#include <unistd.h>

const int DEPTH = 3;
const int INF = 1e9;
const float NOT_CALCULATED_EVAL = 1234567890.f;

void create_AI();

class NODE{
private:
    NODE* parent;
    BOARD* board;
    QVector<NODE*> children;
    float evaluation = NOT_CALCULATED_EVAL;
    MOVE best_move;
public:
    NODE(){};
    NODE(BOARD* cur_board, NODE* _parent);
    NODE* get_parent(){ return parent; };
    BOARD* get_board(){ return board; };
    QVector<NODE*>* get_pointer_to_children(){ return &children; };
    void delete_branch();
    void set_parent(NODE* parent_ptr){ parent = parent_ptr; };
    QPair<MOVE, float> evaluate(int cur_depth, int depth, bool turn, bool initial_turn);
    void create_children();
    int count_children();
    NODE* create_new_node(MOVE move);
    MOVE get_best_move(){ return best_move; };
    void set_best_move(MOVE BM){ best_move = BM; };
    int get_eval_for_alpha_beta_cut();
    ~NODE();
};

class Main_Tree{
private:
    NODE* starting_node;
    int positions_evaluated = 0;
public:
    Main_Tree(){};
    Main_Tree(NODE* _starting_node);
    void trasition_to_new_starting_node(NODE* new_starting_node);
    void recalculate_tree(int depth);
    MOVE get_next_move(){ return starting_node->get_best_move(); };
    void start_tree_from_new_node();
    void increase_amount_of_evaluated_pos(){ positions_evaluated++; };
    ~Main_Tree(){};
};

#endif // AI_H
