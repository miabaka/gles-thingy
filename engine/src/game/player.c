#include "player.h"

void Player_init(Player *this) {
	this->state = PlayerState_Idle;
	this->direction = Direction_None;
	this->x = 0;
	this->y = 0;
}

void Player_setPosition(Player *this, int x, int y) {
	this->x = x;
	this->y = y;
}

void Player_setDirection(Player *this, Direction direction) {
	if (this->state == PlayerState_SeaMoving && Direction_isOpposite(this->direction, direction))
		return;

	this->direction = direction;
}

PlayerUpdateResult Player_update(Player *this, const Playfield *field) {
	if (this->direction == Direction_None || this->state == PlayerState_Dead)
		return PlayerUpdateResult_None;

	int velX, velY;
	Direction_getVelocity(this->direction, &velX, &velY);

	int newX = this->x + velX;
	int newY = this->y + velY;

	if (!Playfield_hasPoint(field, newX, newY))
		return PlayerUpdateResult_None;

	Tile currentTile = Playfield_getTile(field, newX, newY);

	if (currentTile == Tile_PlayerTrace) {
		this->state = PlayerState_Dead;
		return PlayerUpdateResult_Died;
	}

	this->x = newX;
	this->y = newY;

	if (this->state == PlayerState_Idle)
		this->state = PlayerState_LandMoving;

	if (this->state == PlayerState_LandMoving && currentTile == Tile_Sea)
		this->state = PlayerState_SeaMoving;

	if (this->state != PlayerState_SeaMoving)
		return PlayerUpdateResult_None;

	if (currentTile == Tile_Land) {
		this->state = PlayerState_Idle;
		this->direction = Direction_None;
		return PlayerUpdateResult_EnteredLand;
	}

	return PlayerUpdateResult_SeaMove;
}

bool Player_isAlive(const Player *this) {
	return this->state != PlayerState_Dead;
}

void Player_kill(Player *this) {
	this->state = PlayerState_Dead;
}