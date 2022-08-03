#include "perft.h"


PerftInfo::PerftInfo(const uint_fast8_t depth) : search_depth(depth)
{

}

void PerftInfo::ResetData() noexcept
{
    nodes = 0;
    captures = 0;
    en_passant_moves = 0;
    castle_moves = 0;
    promotions = 0;
    checks = 0;
    checkmates = 0;
}

//Performs loading data into PerftInfo struct from json file.
//If couldn't perform opening file by given path, will return false
//In case of success will return true
bool PerftInfo::LoadDataFromJson(const QString &path)
{
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "File was not opened in function PerftInfo::LoadDataFromJson(path), @path: "
                 << path << ", error string: " << file.errorString();
        return false;
    }
    QString text = file.readAll();
    file.close();
    QJsonDocument json_text = QJsonDocument::fromJson(text.toUtf8());
    QJsonObject json_main_obj = json_text.object();
    nodes = json_main_obj["nodes"].toInteger();
    captures = json_main_obj["captures"].toInteger();
    en_passant_moves = json_main_obj["en_passant_moves"].toInteger();
    castle_moves = json_main_obj["castle_moves"].toInteger();
    promotions = json_main_obj["promotions"].toInteger();
    checks = json_main_obj["checks"].toInteger();
    checkmates = json_main_obj["checkmates"].toInteger();
    return true;
}

std::pair<bool, QString> PerftInfo::operator==(const PerftInfo &other) const noexcept{
    QString str_info;
    bool equal = true;
    //Search depth
    str_info += ("Search depth: " + QString::number(search_depth) + " ");
    if(search_depth == other.search_depth){
        str_info += "==";
    }
    else{
        str_info += "+=";
        equal = false;
    }
    str_info += (" " + QString::number(other.search_depth) + "\n");

    //Nodes
    str_info += ("Nodes visited: " + QString::number(nodes) + " ");
    if(nodes == other.nodes){
        str_info += "==";
    }
    else{
        str_info += "+=";
        equal = false;
    }
    str_info += (" " + QString::number(other.nodes) + "\n");

    //Captures
    str_info += ("Captures count: " + QString::number(captures) + " ");
    if(captures == other.captures){
        str_info += "==";
    }
    else{
        str_info += "+=";
        equal = false;
    }
    str_info += (" " + QString::number(other.captures) + "\n");

    //En passant moves
    str_info += ("En passant moves amount: " + QString::number(en_passant_moves) + " ");
    if(en_passant_moves == other.en_passant_moves){
        str_info += "==";
    }
    else{
        str_info += "+=";
        equal = false;
    }
    str_info += (" " + QString::number(other.en_passant_moves) + "\n");

    //Castle moves
    str_info += ("Castle moves amount: " + QString::number(castle_moves) + " ");
    if(castle_moves == other.castle_moves){
        str_info += "==";
    }
    else{
        str_info += "+=";
        equal = false;
    }
    str_info += (" " + QString::number(other.castle_moves) + "\n");

    //Promotions
    str_info += ("Promotion moves amount: " + QString::number(promotions) + " ");
    if(promotions == other.promotions){
        str_info += "==";
    }
    else{
        str_info += "+=";
        equal = false;
    }
    str_info += (" " + QString::number(other.promotions) + "\n");

    //Checks
    str_info += ("Check moves amount: " + QString::number(checks) + " ");
    if(checks == other.checks){
        str_info += "==";
    }
    else{
        str_info += "+=";
        equal = false;
    }
    str_info += (" " + QString::number(other.checks) + "\n");

    //Checkmates
    str_info += ("Checkmates amount: " + QString::number(checkmates) + " ");
    if(checkmates == other.checkmates){
        str_info += "==";
    }
    else{
        str_info += "+=";
        equal = false;
    }
    str_info += (" " + QString::number(other.checkmates) + "\n");
    return {equal, str_info};
}

std::pair<bool, QString> PerftInfo::operator!=(const PerftInfo &other) const noexcept{
    auto[result, str_info] = ((*this) == other);
    return {!result, str_info};
}
