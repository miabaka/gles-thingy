#include "direction.h"

void Direction_getVelocity(Direction this, int *x, int *y) {
    *x = 0;
    *y = 0;

    switch (this) {
        case Direction_Left:
            *x = -1;
            break;

        case Direction_Right:
            *x = 1;
            break;

        case Direction_Up:
            *y = -1;
            break;

        case Direction_Down:
            *y = 1;
            break;

        default:
            break;
    }
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