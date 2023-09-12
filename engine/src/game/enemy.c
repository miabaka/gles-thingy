#include "enemy.h"

#include <stdbool.h>

void Enemy_init(Enemy *this) {
	this->type = EnemyType_Disabled;
	this->x = 0;
	this->y = 0;
	this->velX = 1;
	this->velY = 1;
}

static bool canMove(const Enemy *enemy, const Playfield *field, int dx, int dy) {
	return Playfield_getTile(field, enemy->x + dx, enemy->y + dy) == Tile_Sea;
}

static bool hasTouchedTrace(const Enemy *enemy, const Playfield *field) {
	return Playfield_getTile(field, enemy->x + enemy->velX, enemy->y + enemy->velY) == Tile_PlayerTrace;
}

static void updateVelocity(Enemy *enemy, const Playfield *field) {
	int velX = enemy->velX;
	int velY = enemy->velY;

	if (canMove(enemy, field, velX, velY))
		return;

	if (canMove(enemy, field, -velX, velY)) {
		enemy->velX = -velX;
	} else if (canMove(enemy, field, velX, -velY)) {
		enemy->velY = -velY;
	} else {
		enemy->velX = -velX;
		enemy->velY = -velY;
	}
}

static void applyVelocity(Enemy *enemy) {
	enemy->x += enemy->velX;
	enemy->y += enemy->velY;
}

EnemyUpdateResult Enemy_update(Enemy *this, const Playfield *field) {
	if (this->type != EnemyType_Sea)
		return EnemyUpdateResult_None;

	bool touchedTrace = hasTouchedTrace(this, field);

	updateVelocity(this, field);
	applyVelocity(this);

	return touchedTrace ? EnemyUpdateResult_TouchedTrace : EnemyUpdateResult_None;
}