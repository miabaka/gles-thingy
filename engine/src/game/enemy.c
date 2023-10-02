#include "enemy.h"

#include <stdbool.h>

void Enemy_init(Enemy *this) {
	this->type = EnemyType_Disabled;
	this->position = (ivec2) {0, 0};
	this->velocity = (ivec2) {1, 1};
}

static bool canMove(const Enemy *enemy, const Playfield *field, ivec2 offset) {
	return Playfield_getTile(field, ivec2_add(enemy->position, offset)) == Tile_Sea;
}

static void updateVelocity(Enemy *enemy, const Playfield *field) {
	ivec2 velocity = enemy->velocity;

	if (canMove(enemy, field, velocity))
		return;

	if (canMove(enemy, field, (ivec2) {-velocity.x, velocity.y})) {
		enemy->velocity.x = -velocity.x;
	} else if (canMove(enemy, field, (ivec2) {velocity.x, -velocity.y})) {
		enemy->velocity.y = -velocity.y;
	} else {
		enemy->velocity = (ivec2) {-velocity.x, -velocity.y};
	}
}

static void applyVelocity(Enemy *enemy) {
	enemy->position = ivec2_add(enemy->position, enemy->velocity);
}

void Enemy_update(Enemy *this, const Playfield *field) {
	if (this->type != EnemyType_Sea)
		return;

	updateVelocity(this, field);
	applyVelocity(this);
}