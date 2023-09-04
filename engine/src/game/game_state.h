#pragma once

#include <stdint.h>

#include "enemy.h"
#include "input_state.h"
#include "player.h"
#include "playfield.h"

typedef struct {
    Playfield field;
    Player player;
    Enemy enemies[32];
    float _timeSinceLastUpdate;
} GameState;

bool GameState_init(GameState *, uint8_t fieldWidth, uint8_t fieldHeight);

void GameState_destroy(GameState *);

void GameState_update(GameState *, float timeDelta);

void GameState_applyInputState(GameState *, const InputState *input);