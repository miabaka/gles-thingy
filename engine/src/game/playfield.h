#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../math/ivec2.h"

typedef enum __attribute__((packed)) {
	// Normal tiles
	Tile_Sea = 0,
	Tile_Land,
	Tile_PlayerTrace,
	// Normal-ish tiles tiles for rendering
	Tile_PlayerHead,
	Tile_SeaEnemy,
	Tile_LandEnemy,

	// Special tiles for things like filling
	Tile_FillVisitedSea = 10
} Tile;

typedef struct {
	Tile *tiles;
	uint8_t width;
	uint8_t height;
} Playfield;

bool Playfield_init(Playfield *, uint8_t width, uint8_t height);

void Playfield_destroy(Playfield *);

void Playfield_reset(Playfield *);

size_t Playfield_getSizeTiles(const Playfield *);

size_t Playfield_getSizeBytes(const Playfield *);

bool Playfield_hasPoint(const Playfield *, ivec2 position);

Tile Playfield_getTile(const Playfield *, ivec2 position);

void Playfield_setTile(Playfield *, ivec2 position, Tile tile);

Tile Playfield_exchangeTile(Playfield *, ivec2 position, Tile newTile);

void Playfield_replaceTile(Playfield *, Tile oldTile, Tile newTile);

bool Playfield_fill(Playfield *, ivec2 position, Tile tile);

float Playfield_computeTileFraction(const Playfield *this, Tile tile);