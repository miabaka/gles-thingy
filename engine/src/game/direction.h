#pragma once

#include <stdbool.h>
#include "../math/ivec2.h"

typedef enum {
	Direction_None = 0,
	Direction_Left,
	Direction_Right,
	Direction_Up,
	Direction_Down
} Direction;

ivec2 Direction_toVelocity(Direction);

bool Direction_isOpposite(Direction, Direction other);