#ifndef PIECEWIDGET_H
#define PIECEWIDGET_H

#include <cstdint>

#include <QLabel>

class PieceWidget : public QLabel
{
public:
    PieceWidget(int_fast8_t type);
    ~PieceWidget();

    static QPixmap *InitPixmap(int type);
private:
    int_fast8_t type = 0;
    QPixmap* pixmap = nullptr;
};

#endif // PIECEWIDGET_H
