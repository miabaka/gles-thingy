#pragma once

#include "../math/ivec2.h"
#include "playfield.h"

typedef enum {
	EnemyType_Disabled,
	EnemyType_Sea,
	EnemyType_Land
} EnemyType;

typedef struct {
	EnemyType type;
	ivec2 position;
	ivec2 velocity;
} Enemy;

void Enemy_init(Enemy *);

void Enemy_update(Enemy *, const Playfield *field);