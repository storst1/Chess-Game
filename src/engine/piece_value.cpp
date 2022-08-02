#include "piece_value.h"

PieceValue::PieceValue(const QString& path)
{
    QFile file;
    file.setFileName(path);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString text = file.readAll();
    file.close();
    QJsonDocument json_text = QJsonDocument::fromJson(text.toUtf8());
    QJsonObject json_main_obj = json_text.object();

    pawn_cost = json_main_obj["pawn"].toInt();
    knight_cost = json_main_obj["knight"].toInt();
    bishop_cost = json_main_obj["bishop"].toInt();
    rook_cost = json_main_obj["rook"].toInt();
    queen_cost = json_main_obj["queen"].toInt();
    king_cost = json_main_obj["king"].toInt();
}

int PieceValue::GetPieceValue(int_fast8_t piece)
{
    if(abs(piece) == 1){
        return pawn_cost;
    }
    else if(abs(piece) == 2){
        return knight_cost;
    }
    else if(abs(piece) == 3){
        return bishop_cost;
    }
    else if(abs(piece) == 4){
        return rook_cost;
    }
    else if(abs(piece) == 5){
        return queen_cost;
    }
    else if(abs(piece) == 6){
        return king_cost;
    }
    return 0;
}

void PieceValue::D_PrintPieceValues() const noexcept
{
    QDebug debug = qDebug();
    debug << "Piece values:"
          << "\nPawn: " << pawn_cost
          << "\nKnight: " << knight_cost
          << "\nBishop: " << bishop_cost
          << "\nRook: " << rook_cost
          << "\nQueen: " << queen_cost
          << "\nKing: " << king_cost;
}
