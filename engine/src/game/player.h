#pragma once

#include <stdint.h>

typedef struct {
    uint8_t x;
    uint8_t y;
} Player;

void Player_init(Player *);