#include "enemy.h"

void Enemy_init(Enemy *this) {
	this->type = EnemyType_Disabled;
	this->x = 0;
	this->y = 0;
}