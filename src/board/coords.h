#ifndef COORDS_H
#define COORDS_H

#include <cstdint>

struct Coords{
    uint_fast8_t x;
    uint_fast8_t y;

    bool operator==(const Coords& other) const noexcept;
    bool operator!=(const Coords& other) const noexcept;

    bool Valid() const noexcept;
};

#endif // COORDS_H
