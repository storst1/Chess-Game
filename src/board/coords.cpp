#include "coords.h"

bool Coords::Valid() const noexcept
{
    if(x > 7 || y > 7){
        return false;
    }
    return true;
}

bool Coords::operator==(const Coords &other) const noexcept{
    if(x == other.x && y == other.y){
        return true;
    }
    return false;
}

bool Coords::operator!=(const Coords &other) const noexcept{
    return !((*this) == other);
}
