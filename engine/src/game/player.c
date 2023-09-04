#include "player.h"

void Player_init(Player *this) {
    this->state = PlayerState_Idle;
    this->direction = Direction_None;
    this->x = 0;
    this->y = 0;
}

void Player_setDirection(Player *this, Direction direction) {
    if (this->state == PlayerState_SeaMoving && Direction_isOpposite(this->direction, direction))
        return;

    this->direction = direction;
}

void Player_update(Player *this, Playfield *field) {
    if (this->direction == Direction_None || this->state == PlayerState_Died)
        return;

    int velX, velY;
    Direction_getVelocity(this->direction, &velX, &velY);

    int newX = this->x + velX;
    int newY = this->y + velY;

    if (!Playfield_isPositionValid(field, newX, newY))
        return;

    Tile currentTile = Playfield_getTile(field, newX, newY);

    if (currentTile == Tile_PlayerTrace) {
        this->state = PlayerState_Died;
        return;
    }

    this->x = newX;
    this->y = newY;

    switch (this->state) {
        case PlayerState_Idle:
            this->state = PlayerState_LandMoving;
            break;

        case PlayerState_LandMoving:
            if (currentTile == Tile_Sea)
                this->state = PlayerState_SeaMoving;

        default:
            break;
    }

    if (this->state != PlayerState_SeaMoving)
        return;

    if (currentTile == Tile_Land) {
        this->state = PlayerState_Idle;
        this->direction = Direction_None;
        return;
    }

    Playfield_setTile(field, newX, newY, Tile_PlayerTrace);
}