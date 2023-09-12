#pragma once

#include <stdint.h>

typedef enum {
	EnemyType_Disabled,
	EnemyType_Sea,
	EnemyType_Land
} EnemyType;

typedef struct {
	EnemyType type;
	uint8_t x;
	uint8_t y;
} Enemy;

void Enemy_init(Enemy *);