#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QLayout>
#include <QPixmap>
#include <QMouseEvent>
#include <QPainter>

#include "board.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void mousePressEvent(QMouseEvent *event);
private:
    Ui::MainWindow *ui;
    QGridLayout* lay;
    QLabel* main_label;
    QPixmap* pixmap;
    QPainter* painter;
    Board board;
    int chosen_x = -1;
    int chosen_y = -1;
};
#endif // MAINWINDOW_H
