#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "board.h"
#include "AI.h"

bool AI_used;
bool AI_turn;
int board_x;
int board_y;
float last_move_eval_time;

extern QApplication a;
extern BOARD* board;
extern QUEEN* queen;

QColor* Black_Square;
QColor* White_Square;
QColor* Highlighted_Square;
QColor* Possibly_Highlighted_Square;
QColor* Check_Highlighted_Square;
QColor* Last_Move_Highlight;
int w;
int h;

extern int positions_evaluated;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setup_screen();
    create_objects();
    ui->board_label->hide();
}

void MainWindow::create_objects(){
    White_Square = new QColor(255, 206, 158);
    Black_Square = new QColor(209, 139, 71);
    Highlighted_Square = new QColor(1, 0, 128);
    Possibly_Highlighted_Square = new QColor(76, 136, 186);
    Check_Highlighted_Square = new QColor(255, 0, 0);
    Last_Move_Highlight = new QColor(250, 227, 103);

    create_board_objects();
    RePrintBoard();
}

void MainWindow::setup_screen(){
    ui->eval_time_label->hide();
    ui->eval_time_label->setWordWrap(true);
    ui->checkmate_label->hide();
    w = ui->board_label->width();
    h = ui->board_label->height();
    board_x = ui->board_label->x();
    board_y = ui->board_label->y();
    pix = new QPixmap (QPixmap ("./Images/board.png").scaled(600, 600, Qt::KeepAspectRatio));
    painter = new QPainter(pix);
    ui->board_label->setPixmap((*pix).scaled(w, h, Qt::KeepAspectRatio));
}
void MainWindow::RePrintBoard(){
    board->print_board(painter);
    ui->board_label->setPixmap((*pix).scaled(w, h, Qt::KeepAspectRatio));
}

void MainWindow::on_StartButton_clicked()
{
    AI_used = false;
    ui->StartButton->hide();
    ui->StartButton_AI->hide();
    ui->board_label->show();
}

void MainWindow::on_StartButton_AI_clicked()
{
    AI_used = true;
    AI_turn = false;
    ui->StartButton->hide();
    ui->StartButton_AI->hide();
    ui->board_label->show();
    ui->eval_time_label->setText("");
    ui->eval_time_label->show();
    create_AI();
    //procces_promotion();
}

void MainWindow::mousePressEvent(QMouseEvent *event){
    int x = event->globalPosition().x();
    int y = event->globalPosition().y();
    if(!(board->on_main_board(x, y))){
        return;
    }
    int moved = board->mouse_click(x, y);
    int game_status = 100;
    RePrintBoard();
    if(moved){
        game_status = board->proceed_game(true, true, false);
    }
    RePrintBoard();
    if(game_status != 100){
        if(game_status != 0){
            ui->checkmate_label->show();
        }
        else{
            ui->checkmate_label->setText("Stalemate!");
            ui->checkmate_label->show();
        }
        delete board;
        return;
    }
    if(moved && board->get_turn() == AI_turn && AI_used){
        //board->make_AI_move();
        board->make_BETTER_AI_move();
        game_status = board->proceed_game(true, true, false);
        if(game_status != 100){
            if(game_status != 0){
                ui->checkmate_label->show();
            }
            else{
                ui->checkmate_label->setText("Stalemate!");
                ui->checkmate_label->show();
            }
        }
        RePrintBoard();
        QString time_passed = QString::fromStdString(std::to_string(last_move_eval_time));
        QString text = QString::fromStdString(std::to_string(positions_evaluated));
        text += " positions evaluated in: ";
        text += time_passed;
        text += " sec";
        ui->eval_time_label->setText(text);
    }
}


int MainWindow::procces_promotion(){
    /*
    QPixmap prom_pixmap(300, 75);
    QPainter prom_painter(&prom_pixmap);
    prom_painter.setWindow(0, 0, 300, 75);
    //prom_painter.drawPixmap(0, 0, queen->BLACK().scaled(75, 75, Qt::KeepAspectRatio));
    //paint->drawPixmap(i*sq_side, j*sq_side, b[i][j]->get_piece()->get_piece_img()->scaled(sq_side, sq_side, Qt::KeepAspectRatio))
    */
    return 0;
}


MainWindow::~MainWindow()
{
    delete ui;
}

