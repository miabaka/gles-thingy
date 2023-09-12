#pragma once

#include "playfield.h"

typedef enum {
	EnemyType_Disabled,
	EnemyType_Sea,
	EnemyType_Land
} EnemyType;

typedef struct {
	EnemyType type;
	int x;
	int y;
	int velX;
	int velY;
} Enemy;

typedef enum {
	EnemyUpdateResult_None,
	EnemyUpdateResult_TouchedTrace
} EnemyUpdateResult;

void Enemy_init(Enemy *);

EnemyUpdateResult Enemy_update(Enemy *, const Playfield *field);