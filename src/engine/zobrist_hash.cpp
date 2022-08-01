#include "zobrist_hash.h"
#include "board/board.h"

//Creates uninitialized object that is NOT ready to be used properly
Zobrist_Hash::Zobrist_Hash()
{

}

//Initializes zobrish hash object with random values
void Zobrist_Hash::Init() noexcept
{
    for(int i = 0; i < 12; ++i){
        for(int j = 0; j < 8; ++j){
            for(int g = 0; g < 8; ++g){
                pieces_hash[i][j][g] = QRandomGenerator::global()->generate64();
            }
        }
    }
    for(int i = 0; i < 8; ++i){
        en_passant_files[i] = QRandomGenerator::global()->generate64();
    }
    black_move = QRandomGenerator::global()->generate64();
    white_castle_kingside = QRandomGenerator::global()->generate64();
    white_castle_queenside = QRandomGenerator::global()->generate64();
    black_castle_kingside = QRandomGenerator::global()->generate64();
    black_castle_queenside = QRandomGenerator::global()->generate64();
}

bool Zobrist_Hash::IsInitValidly() const noexcept
{
    return initialized_correctly;
}

//Calculates Zobrist hash for given position on board
uint_fast64_t Zobrist_Hash::CalculateHashValue(Board &board) noexcept
{
    uint_fast64_t hash = 0;
    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            hash ^= PieceHash(board.Get(i, j), i, j);
        }
    }

    if(!board.Turn()){
        hash ^= black_move;
    }
    if(board.WCK_Possible()){
        hash ^= white_castle_kingside;
    }
    if(board.WCQ_Possible()){
        hash ^= white_castle_queenside;
    }
    if(board.BCK_Possible()){
        hash ^= black_castle_kingside;
    }
    if(board.BCQ_Possible()){
        hash ^= black_castle_queenside;
    }

    for(const auto& move : board.PossibleMovesRef()){
        if(move.ep_x != 8){
            hash ^= en_passant_files[move.x2];
            break;
        }
    }

    return hash;
}

uint_fast64_t &Zobrist_Hash::PieceHash(int_fast8_t piece, uint_fast8_t x, uint_fast8_t y) noexcept
{
    if(piece < 0){
        piece += 13;
    }
    return pieces_hash[piece - 1][x][y];
}

uint_fast64_t &Zobrist_Hash::Move() noexcept
{
    return black_move;
}

uint_fast64_t &Zobrist_Hash::WCK() noexcept
{
    return white_castle_kingside;
}

uint_fast64_t &Zobrist_Hash::WCQ() noexcept
{
    return white_castle_queenside;
}

uint_fast64_t &Zobrist_Hash::BCK() noexcept
{
    return black_castle_kingside;
}

uint_fast64_t &Zobrist_Hash::BCQ() noexcept
{
    return black_castle_queenside;
}

void Zobrist_Hash::SaveAsJsonFile(QString path) const
{
    QJsonObject main_obj;
    QJsonObject pieces;
    QJsonObject info;
    QJsonObject en_passant;
    for(int i = 0; i < 12; ++i){
        for(int j = 0; j < 8; ++j){
            for(int u = 0; u < 8; ++u){
                QString value_name = QString::number(i) + ":" + QString::number(j) + ":" + QString::number(u);
                pieces.insert(value_name, QString::number((qulonglong)pieces_hash[i][j][u]));
            }
        }
    }
    for(int i = 0; i < 8; ++i){
        en_passant.insert(QString::number(i), QString::number((qulonglong)en_passant_files[i]));
    }
    info.insert("move", QString::number((qulonglong)black_move));
    info.insert("white_castle_kingside", QString::number((qulonglong)white_castle_kingside));
    info.insert("white_castle_queenside", QString::number((qulonglong)white_castle_queenside));
    info.insert("black_castle_kingside", QString::number((qulonglong)black_castle_kingside));
    info.insert("black_castle_queenside", QString::number((qulonglong)black_castle_queenside));

    main_obj.insert("info", info);
    main_obj.insert("en_passant", en_passant);
    main_obj.insert("pieces_hash", pieces);

    QJsonDocument document;
    document.setObject(main_obj);
    QByteArray byte_array = document.toJson(QJsonDocument::Indented);

    QFile file(path);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)){
        QTextStream istream(&file);
        istream << byte_array;
        file.close();
    }
    else{
        //Error occured during the operation of opening the file
        qDebug() << "File was not opened in function SaveAsJsonFile(), path: " << path
                 << ", error string: " << file.errorString();
        return;
    }

}

void Zobrist_Hash::LoadFromJsonFile(QString path)
{
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Failed to open the file in function ZobristHash::LoadFromJsonFile(), path: " << path
                 << ", error message: " << file.errorString();
        return;
    }
    //File was opened correctly
    initialized_correctly = true;
    QString text = file.readAll();
    file.close();

    QJsonDocument json_doc = QJsonDocument::fromJson(text.toUtf8());
    QJsonObject main_obj = json_doc.object();
    QJsonObject pieces_hash_obj = main_obj["pieces_hash"].toObject();
    QJsonObject en_passang_obj = main_obj["en_passant"].toObject();
    QJsonObject info_obj = main_obj["info"].toObject();

    //static_assert (sizeof(uint_fast64_t) == sizeof(unsigned long long));

    for(int i = 0; i < 12; ++i){
        for(int j = 0; j < 8; ++j){
            for(int u = 0 ;u < 8; ++u){
                QString value_name = QString::number(i) + ":" + QString::number(j) + ":" + QString::number(u);
                pieces_hash[i][j][u] = pieces_hash_obj[value_name].toString().toULongLong();
            }
        }
    }

    for(int i = 0; i < 8; ++i){
        en_passant_files[i] = en_passang_obj[QString::number(i)].toString().toULongLong();
    }

    black_move = info_obj["move"].toString().toULongLong();
    white_castle_kingside = info_obj["white_castle_kingside"].toString().toULongLong();
    white_castle_queenside = info_obj["white_castle_queenside"].toString().toULongLong();
    black_castle_kingside = info_obj["black_castle_kingside"].toString().toULongLong();
    black_castle_queenside = info_obj["black_castle_queenside"].toString().toULongLong();
}

QString Zobrist_Hash::DefaultZHPath() noexcept
{
    QDir exe_dir = QDir::current();
    exe_dir.cdUp();
    QString main_dir_path = exe_dir.path();
    return main_dir_path + "/src/vars/zobrist_hash_default.json";
}
