#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//Qt Includes
#include <QApplication>
#include <QMainWindow>
#include <QPixmap>
#include <QLabel>
#include <QPainter>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QDrag>
#include <QColor>
#include <QDebug>
#include <QThread>
#include <QFile>
#include <QByteArray>
//C++ libraries includes
#include <vector>
#include <string>
#include <iterator>
#include <fstream>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    void RePrintBoard();
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_StartButton_clicked();
    void create_objects();
    void setup_screen();
    void mousePressEvent(QMouseEvent *event) override;

    void on_StartButton_AI_clicked();
    int procces_promotion();

private:
    Ui::MainWindow *ui;
    QPixmap *pix;
    QPainter *painter;
};
#endif // MAINWINDOW_H
