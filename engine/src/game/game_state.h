#pragma once

#include <stdint.h>

#include "enemy.h"
#include "player.h"
#include "playfield.h"

typedef struct {
    Playfield field;
    Player player;
    Enemy enemies[32];
} GameState;

bool GameState_init(GameState *, uint8_t fieldWidth, uint8_t fieldHeight);

void GameState_destroy(GameState *);