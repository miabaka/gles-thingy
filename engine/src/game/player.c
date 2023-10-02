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
	this->position = (ivec2) {0, 0};
}

void Player_setPosition(Player *this, ivec2 position) {
	this->position = position;
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

	ivec2 newPosition = ivec2_add(this->position, Direction_toVelocity(this->direction));

	if (!Playfield_hasPoint(field, newPosition))
		return PlayerUpdateResult_None;

	Tile currentTile = Playfield_getTile(field, newPosition);

	this->position = newPosition;

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

	ivec2 newPosition = ivec2_add(this->position, Direction_toVelocity(this->direction));

	return Playfield_getTile(field, newPosition) == Tile_PlayerTrace;
}

bool Player_isAlive(const Player *this) {
	return this->_lives > 0;
}

void Player_kill(Player *this) {
	this->_mustDie = true;
}