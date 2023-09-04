#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum : uint8_t {
    Tile_Sea = 0,
    Tile_Land,
    Tile_PlayerTrace,
    Tile_PlayerHead,
    Tile_SeaEnemy,
    Tile_LandEnemy
} Tile;

typedef struct {
    Tile *tiles;
    uint8_t width;
    uint8_t height;
} Playfield;

bool Playfield_init(Playfield *, uint8_t width, uint8_t height);

void Playfield_destroy(Playfield *);

void Playfield_reset(Playfield *);

size_t Playfield_getSizeBytes(const Playfield *);