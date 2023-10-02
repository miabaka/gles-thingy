#include "direction.h"

ivec2 Direction_toVelocity(Direction this) {
	ivec2 velocity = {0, 0};

	switch (this) {
		case Direction_Left:
			velocity.x = -1;
			break;

		case Direction_Right:
			velocity.x = 1;
			break;

		case Direction_Up:
			velocity.y = -1;
			break;

		case Direction_Down:
			velocity.y = 1;
			break;

		default:
			break;
	}

	return velocity;
}

bool Direction_isOpposite(Direction this, Direction other) {
	switch (this) {
		case Direction_Left:
			return other == Direction_Right;

		case Direction_Right:
			return other == Direction_Left;

		case Direction_Up:
			return other == Direction_Down;

		case Direction_Down:
			return other == Direction_Up;

		default:
			return false;
	}
}