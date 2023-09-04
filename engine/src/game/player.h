#pragma once

#include <stdint.h>

#include "direction.h"
#include "playfield.h"

typedef enum {
    PlayerState_Idle,
    PlayerState_LandMoving,
    PlayerState_SeaMoving,
    PlayerState_Died
} PlayerState;

typedef struct {
    PlayerState state;
    Direction direction;
    int x;
    int y;
} Player;

void Player_init(Player *);

void Player_setDirection(Player *, Direction direction);

void Player_update(Player *, Playfield *field);
