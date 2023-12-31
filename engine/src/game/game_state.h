#pragma once

#include <stdint.h>

#include "../math/ivec2.h"
#include "enemy.h"
#include "input_state.h"
#include "player.h"
#include "playfield.h"

typedef struct {
	Playfield field;
	Player player;
	Enemy enemies[32];
	float _timeSinceLastUpdate;
	Tile _tileUnderPlayer;
	Tile _tilesUnderEnemies[32];
	ivec2 _landPosition;
	InputState _inputState;
	bool _inputHandled;
} GameState;

bool GameState_init(GameState *, uint8_t fieldWidth, uint8_t fieldHeight);

void GameState_destroy(GameState *);

void GameState_update(GameState *, float timeDelta);

void GameState_setInputState(GameState *, const InputState *input);

void GameState_bakeDynamicObjects(GameState *);

void GameState_unbakeDynamicObjects(GameState *);