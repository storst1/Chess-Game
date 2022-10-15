#include <AI.h>
#include <board.h>

extern BOARD* board;
Main_Tree* AI_MAIN_TREE;
extern float last_move_eval_time;

void create_AI(){
    AI_MAIN_TREE = new Main_Tree();
    AI_MAIN_TREE->start_tree_from_new_node();
}

NODE::NODE(BOARD* cur_board, NODE* _parent){
    board = cur_board; //board should be puten together
    parent = _parent;
}

void NODE::delete_branch(){
    for(int i = 0; i < (int)children.size(); ++i){
        children[i]->delete_branch();
    }
    delete this;
}

QPair<MOVE, float> NODE::evaluate(int cur_depth, int depth, bool turn, bool initial_turn){
    AI_MAIN_TREE->increase_amount_of_evaluated_pos();
    if((cur_depth == depth - 1 && initial_turn != turn) || (board->is_checkmate(true) || board->is_stalemate())){
        //Depth lim is reached or game's over
        evaluation = board->get_evaluation();
        if(!initial_turn){
            //evaluation = -1 * evaluation;
        }
        QPair<MOVE, int> to_ret = {MOVE(0, 0, board), evaluation};
        return to_ret;
    }
    //Increasing depth
    //Transition to new position
    //create_children();
    float worst = INF;
    MOVE cur_best_move;
    int depth_increase = (turn == initial_turn) ? 1 : 0;
    /*
    for(int i = 0; i < (int)children.size(); ++i){
        children[i]->evaluate(cur_depth + depth_increase, depth, !turn, initial_turn);
        if(children[i]->evaluation < worst){
            worst = children[i]->evaluation;
            cur_best_move = children[i]->board->get_last_move();
        }
    }
    */

    //int cur_alpha_beta_cut_val = get_eval_for_alpha_beta_cut();

    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            if(!board->get_square(i, j)->empty() && board->get_turn() == board->get_square(i, j)->get_piece()->is_white()){
                QVector<SQUARE*> cur_possible_moves = board->get_square(i, j)->get_possible_moves();
                for(int u = 0; u < (int)cur_possible_moves.size(); ++u){
                    NODE* cur_node = create_new_node(MOVE(board->get_square(i, j), cur_possible_moves[u], board));
                    QPair<MOVE, float> cur = cur_node->evaluate(cur_depth + depth_increase, depth, !turn, initial_turn);
                    if(cur.second < worst){
                        worst = cur.second;
                        cur_best_move = {board->get_square(i, j), cur_possible_moves[u], board};
                    }
                    delete cur_node;
                }
            }
        }
    }
    best_move = cur_best_move;
    evaluation = -worst;

    for(int i = 0; i < (int)children.size(); ++i){
        delete children[i];
    }

    return {cur_best_move, -worst};
}

void NODE::create_children(){
    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            if(!board->get_square(i, j)->empty() && board->get_turn() == board->get_square(i, j)->get_piece()->is_white()){
                QVector<SQUARE*> cur_possible_moves = board->get_square(i, j)->get_possible_moves();
                for(int u = 0; u < (int)cur_possible_moves.size(); ++u){
                    children.push_back(create_new_node(MOVE(board->get_square(i, j), cur_possible_moves[u], board)));
                }
            }
        }
    }
}

int NODE::count_children(){
    int cnt = 0;
    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            if(!board->get_square(i, j)->empty() && board->get_turn() == board->get_square(i, j)->get_piece()->is_white()){
                cnt += board->get_square(i, j)->get_possible_moves().size();
            }
        }
    }
    return cnt;
}

NODE *NODE::create_new_node(MOVE move){
    BOARD* new_board = board->transform_to_next_position(board, move, true, false);
    NODE* new_node = new NODE(new_board, this);
    return new_node;
}

NODE::~NODE(){
    delete board;
}

Main_Tree::Main_Tree(NODE* _starting_node){
    starting_node = _starting_node;
}

void Main_Tree::trasition_to_new_starting_node(NODE *new_starting_node){
    QVector<NODE*>* cur_children = starting_node->get_pointer_to_children();
    for(int i = 0; i < (int)cur_children->size(); ++i){
        if((*cur_children)[i] != new_starting_node){
            (*cur_children)[i]->delete_branch();
            cur_children->erase(cur_children->begin() + i, cur_children->begin() + i + 1);
            --i;
        }
    }

    starting_node = new_starting_node;
    starting_node->set_parent(nullptr);
    recalculate_tree(DEPTH);
}


void Main_Tree::recalculate_tree(int depth){
    auto start = std::chrono::steady_clock::now();
    positions_evaluated = 0;
    QPair<MOVE, float> best_var = starting_node->evaluate(0, depth, starting_node->get_board()->get_turn(), starting_node->get_board()->get_turn());
    starting_node->set_best_move(best_var.first);
    auto end = std::chrono::steady_clock::now();
    double time_passed_in_sec = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 1000.0;
    qDebug() << positions_evaluated << " positions evaluated in " << time_passed_in_sec << " seconds \n";
    last_move_eval_time = time_passed_in_sec;
}

void Main_Tree::start_tree_from_new_node(){
    //BOARD* AI_starting_board = board->copy_board(board);
    NODE* new_starting_node = new NODE(board, nullptr);
    starting_node = new_starting_node;
}

int NODE::get_eval_for_alpha_beta_cut(){
    NODE* cur_node = this;
    if(cur_node->parent == nullptr){
        return NOT_CALCULATED_EVAL;
    }
    /*
    cur_node = cur_node->parent;
    if(cur_node->parent == nullptr){
        return NOT_CALCULATED_EVAL;
    }
    */
    cur_node = cur_node->parent;
    return cur_node->evaluation;
}
