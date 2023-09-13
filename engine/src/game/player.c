#include "player.h"

void Player_init(Player *this) {
	Player_reset(this);
}

void Player_reset(Player *this) {
	Player_resetMovement(this);
	this->_lives = 3;
	this->_mustDie = false;
}

void Player_resetMovement(Player *this) {
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
	if (!(Player_isAlive(this) && this->direction != Direction_None))
		return PlayerUpdateResult_None;

	if (this->_mustDie) {
		this->_lives--;
		this->_mustDie = false;
		return PlayerUpdateResult_Died;
	}

	int velX, velY;
	Direction_getVelocity(this->direction, &velX, &velY);

	int newX = this->x + velX;
	int newY = this->y + velY;

	if (!Playfield_hasPoint(field, newX, newY))
		return PlayerUpdateResult_None;

	Tile currentTile = Playfield_getTile(field, newX, newY);

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

bool Player_willTouchTraceNextUpdate(const Player *this, const Playfield *field) {
	if (!Player_isAlive(this))
		return false;

	int velX, velY;
	Direction_getVelocity(this->direction, &velX, &velY);

	int newX = this->x + velX;
	int newY = this->y + velY;

	return Playfield_getTile(field, newX, newY) == Tile_PlayerTrace;
}

bool Player_isAlive(const Player *this) {
	return this->_lives > 0;
}

void Player_kill(Player *this) {
	this->_mustDie = true;
}