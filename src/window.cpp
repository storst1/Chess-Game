#include "window.h"
#include "ui_window.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->checkmate_label->hide();
    lay = new QGridLayout(ui->centralwidget);
    pixmap = new QPixmap(800, 800);
    painter = new QPainter(pixmap);
    board.DrawBoard(painter, -1, -1);
    main_label = new QLabel();
    main_label->setFixedSize(800, 800);
    main_label->setPixmap(*pixmap);
    main_label->setScaledContents(true);
    lay->addWidget(main_label);
    board.CalculatePossibleMoves();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    int x = event->pos().x() - main_label->pos().x();
    int y = event->pos().y() - main_label->pos().y();
    //qDebug() << x << " " << y;
    if(!main_label->geometry().contains(event->pos())){
        //qDebug() << "Not on board!";
        return;
    }
    int i = x / 100;
    int j = y / 100;
    if(chosen_x != -1 && board.IsLegalMove(chosen_x, chosen_y, i, j)){
        //Make move
        board.ApplyUserMove(chosen_x, chosen_y, i, j);
        if(board.IsCheckmate()){
            ui->checkmate_label->show();
        }
        chosen_x = -1;
        board.DrawBoard(painter);
        main_label->setPixmap(*pixmap);
        main_label->update();
        return;
    }

    if(board.CheckIfPieceWakable(i, j)){
        chosen_x = i;
        chosen_y = j;
        board.DrawBoard(painter, i, j);
        main_label->setPixmap(*pixmap);
        main_label->update();
    }
    else{
        chosen_x = -1;
    }
}

