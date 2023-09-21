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

void Enemy_init(Enemy *);

void Enemy_update(Enemy *, const Playfield *field);