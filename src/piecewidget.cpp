#include "piecewidget.h"


PieceWidget::PieceWidget(int_fast8_t type) : type(type)
{
    pixmap = InitPixmap(type);
    setPixmap(*pixmap);
}

PieceWidget::~PieceWidget()
{
    delete pixmap;
}

QPixmap *PieceWidget::InitPixmap(int type)
{
    if(type == 1){
        return new QPixmap(":/White_Pawn.png");
    }
    else if(type == -1){
        return new QPixmap(":/Black_Pawn.png");
    }
    else if(type == 2){
        return new QPixmap(":/White_Knight.png");
    }
    else if(type == -2){
        return new QPixmap(":/Black_Knight.png");
    }
    else if(type == 3){
        return new QPixmap(":/White_Bishop.png");
    }
    else if(type == -3){
        return new QPixmap(":/Black_Bishop.png");
    }
    else if(type == 4){
        return new QPixmap(":/White_Rook.png");
    }
    else if(type == -4){
        return new QPixmap(":/Black_Rook.png");
    }
    else if(type == 5){
        return new QPixmap(":/White_Queen.png");
    }
    else if(type == -5){
        return new QPixmap(":/Black_Queen.png");
    }
    else if(type == 5){
        return new QPixmap(":/White_King.png");
    }
    else if(type == -5){
        return new QPixmap(":/Black_King.png");
    }
    return new QPixmap();
}
