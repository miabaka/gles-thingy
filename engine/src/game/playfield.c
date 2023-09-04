#include "playfield.h"

#include <malloc.h>

bool Playfield_init(Playfield *this, uint8_t width, uint8_t height) {
    this->width = width;
    this->height = height;

    this->tiles = (Tile *) malloc(sizeof(Tile) * width * height);

    if (!this->tiles)
        return false;

    Playfield_reset(this);

    return true;
}

void Playfield_destroy(Playfield *this) {
    free(this->tiles);
}

void Playfield_reset(Playfield *this) {
    for (int nTile = 0; nTile < (this->width * this->height); nTile++) {
        int x = nTile % this->width;
        int y = nTile / this->width;

        bool isLand = (x <= 1 || y <= 1 || x >= (this->width - 2) || y >= (this->height - 2));

        this->tiles[nTile] = isLand ? Tile_Land : Tile_Sea;
    }
}

size_t Playfield_getSizeTiles(const Playfield *this) {
    return this->width * this->height;
}

size_t Playfield_getSizeBytes(const Playfield *this) {
    return sizeof(this->tiles[0]) * this->width * this->height;
}

bool Playfield_isPositionValid(const Playfield *this, int x, int y) {
    return x >= 0 && x < this->width && y >= 0 && y < this->height;
}

Tile Playfield_getTile(const Playfield *this, int x, int y) {
    if (!Playfield_isPositionValid(this, x, y))
        return Tile_Sea;

    return this->tiles[y * this->width + x];
}

void Playfield_setTile(Playfield *this, int x, int y, Tile tile) {
    if (!Playfield_isPositionValid(this, x, y))
        return;

    this->tiles[y * this->width + x] = tile;
}

Tile Playfield_replaceTile(Playfield *this, int x, int y, Tile newTile) {
    if (!Playfield_isPositionValid(this, x, y))
        return Tile_Sea;

    Tile *tile = &this->tiles[y * this->width + x];

    Tile initialTile = *tile;
    *tile = newTile;

    return initialTile;
}

// TODO: implement it as scanline filling
void Playfield_fillSea(Playfield *this, int x, int y) {
    if (Playfield_getTile(this, x, y) != Tile_Sea)
        return;

    Playfield_setTile(this, x, y, Tile_FillVisitedSea);

    Playfield_fillSea(this, x, y + 1);
    Playfield_fillSea(this, x, y - 1);
    Playfield_fillSea(this, x - 1, y);
    Playfield_fillSea(this, x + 1, y);
}