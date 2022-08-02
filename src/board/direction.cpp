#include "direction.h"

Direction::Dir Direction::CalculateDir(Coords from, Coords to) noexcept
{
    int_fast8_t x_diff = from.x - to.x;
    int_fast8_t y_diff = from.y - to.y;
    //UL
    if(x_diff > 0 && y_diff > 0 && x_diff == y_diff){
        return DIR::UL;
    }
    //U
    if(x_diff == 0 && y_diff > 0){
        return DIR::U;
    }
    //UR
    if(x_diff < 0 && y_diff > 0 && -x_diff == y_diff){
        return DIR::UR;
    }
    //R
    if(x_diff < 0 && y_diff == 0){
        return DIR::R;
    }
    //DR
    if(x_diff < 0 && y_diff < 0 && x_diff == y_diff){
        return DIR::DR;
    }
    //D
    if(x_diff == 0 && y_diff < 0){
        return DIR::D;
    }
    //DL
    if(x_diff > 0 && y_diff < 0 && x_diff == -y_diff){
        return DIR::DL;
    }
    //L
    if(x_diff > 0 && y_diff == 0){
        return DIR::L;
    }
    //from == to
    if(x_diff == 0 && y_diff == 0){
        return DIR::NONE;
    }

    //Everything else is considered a knight move
    return DIR::KM;
}

//Returns opposite direction if type of direction is diagonal, horizontal or vertical otherwise returns given direction
Direction::Dir Direction::Opposite(const Dir &dir) noexcept
{
    if(dir == DIR::D){
        return DIR::U;
    }
    if(dir == DIR::DL){
        return DIR::UR;
    }
    if(dir == DIR::DR){
        return DIR::UL;
    }
    if(dir == DIR::L){
        return DIR::R;
    }
    if(dir == DIR::R){
        return DIR::R;
    }
    if(dir == DIR::U){
        return DIR::D;
    }
    if(dir == DIR::UL){
        return DIR::DR;
    }
    if(dir == DIR::UR){
        return DIR::DL;
    }

    return dir;
}
