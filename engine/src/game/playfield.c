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

bool Playfield_hasPoint(const Playfield *this, ivec2 pos) {
	return pos.x >= 0 && pos.x < this->width && pos.y >= 0 && pos.y < this->height;
}

Tile Playfield_getTile(const Playfield *this, ivec2 pos) {
	return Playfield_hasPoint(this, pos) ? this->tiles[pos.y * this->width + pos.x] : Tile_Sea;
}

void Playfield_setTile(Playfield *this, ivec2 pos, Tile tile) {
	if (!Playfield_hasPoint(this, pos))
		return;

	this->tiles[pos.y * this->width + pos.x] = tile;
}

Tile Playfield_exchangeTile(Playfield *this, ivec2 pos, Tile newTile) {
	if (!Playfield_hasPoint(this, pos))
		return Tile_Sea;

	Tile *tile = &this->tiles[pos.y * this->width + pos.x];

	Tile initialTile = *tile;
	*tile = newTile;

	return initialTile;
}

void Playfield_replaceTile(Playfield *this, Tile oldTile, Tile newTile) {
	for (size_t nTile = 0; nTile < Playfield_getSizeTiles(this); nTile++) {
		Tile *tile = &this->tiles[nTile];

		if (*tile != oldTile)
			continue;

		*tile = newTile;
	}
}

float Playfield_computeTileFraction(const Playfield *this, Tile tile) {
	size_t tileCount = 0;
	size_t totalTileCount = Playfield_getSizeTiles(this);

	for (size_t nTile = 0; nTile < totalTileCount; nTile++) {
		if (this->tiles[nTile] == tile)
			tileCount++;
	}

	return (float) tileCount / (float) totalTileCount;
}