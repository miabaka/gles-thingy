#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "../math/ivec2.h"
#include "direction.h"
#include "playfield.h"

typedef enum {
	PlayerState_Idle,
	PlayerState_LandMoving,
	PlayerState_SeaMoving
} PlayerState;

typedef struct {
	PlayerState state;
	Direction direction;
	ivec2 position;
	int _lives;
	bool _mustDie;
} Player;

typedef enum {
	PlayerUpdateResult_None,
	PlayerUpdateResult_SeaMove,
	PlayerUpdateResult_EnteredLand,
	PlayerUpdateResult_Died
} PlayerUpdateResult;

void Player_init(Player *);

void Player_reset(Player *);

void Player_resetMovement(Player *);

void Player_setPosition(Player *, ivec2 position);

void Player_setDirection(Player *, Direction direction);

PlayerUpdateResult Player_update(Player *, const Playfield *field);

bool Player_willTouchTraceNextUpdate(const Player *, const Playfield *field);

bool Player_isAlive(const Player *);

void Player_kill(Player *);