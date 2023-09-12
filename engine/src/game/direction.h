#pragma once

#include <stdbool.h>

typedef enum {
	Direction_None = 0,
	Direction_Left,
	Direction_Right,
	Direction_Up,
	Direction_Down
} Direction;

void Direction_getVelocity(Direction, int *x, int *y);

bool Direction_isOpposite(Direction, Direction other);