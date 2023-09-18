#include "game_state.h"

#include <assert.h>
#include "../utils/common.h"

static void resetPlayer(GameState *state) {
	Player *player = &state->player;
	Player_reset(player);
	Player_setPosition(player, state->_landX, state->_landY);
}

static void reset(GameState *state, bool resetField) {
	if (resetField)
		Playfield_reset(&state->field);

	state->_landX = state->field.width / 2;
	state->_landY = 1;

	state->_timeSinceLastUpdate = 0.f;

	resetPlayer(state);

	for (size_t nEnemy = 0; nEnemy < ARRAY_SIZE(state->enemies); nEnemy++)
		// Actually, multiple initializations is fine because enemies don't allocate
		// any dynamic memory so it acts like an reset
		// TODO: separate reset and init routines
		Enemy_init(&state->enemies[nEnemy]);

	state->enemies[0].type = EnemyType_Sea;
	state->enemies[0].x = 10;
	state->enemies[0].y = 10;

	state->enemies[1].type = EnemyType_Sea;
	state->enemies[1].x = 70;
	state->enemies[1].y = 40;
}

bool GameState_init(GameState *this, uint8_t fieldWidth, uint8_t fieldHeight) {
	this->_inputHandled = true;

	if (!Playfield_init(&this->field, fieldWidth, fieldHeight))
		return false;

	Player_init(&this->player);
	reset(this, false);	

	return true;
}

void GameState_destroy(GameState *this) {
	Playfield_destroy(&this->field);
}

static void fillTracedArea(GameState *state) {
	Playfield *field = &state->field;

	for (size_t nEnemy = 0; nEnemy < ARRAY_SIZE(state->enemies); nEnemy++) {
		const Enemy *enemy = &state->enemies[nEnemy];

		if (enemy->type != EnemyType_Sea)
			continue;

		Playfield_fill(field, enemy->x, enemy->y, Tile_FillVisitedSea);
	}

	for (size_t nTile = 0; nTile < Playfield_getSizeTiles(field); nTile++) {
		Tile *tile = &field->tiles[nTile];

		switch (*tile) {
			case Tile_FillVisitedSea:
				*tile = Tile_Sea;
				break;

			case Tile_Sea:
			case Tile_PlayerTrace:
				*tile = Tile_Land;

			default:
				break;
		}
	}
}

static void returnPlayerToLand(GameState *state) {
	Player *player = &state->player;
	Player_resetMovement(player);
	Player_setPosition(player, state->_landX, state->_landY);
}

static void clearPlayerTrace(GameState *state) {
	Playfield_replaceTile(&state->field, Tile_PlayerTrace, Tile_Sea);
}

static void handlePlayerDeath(GameState *state) {
	if (!Player_isAlive(&state->player)) {
		reset(state, true);
		return;
	}

	returnPlayerToLand(state);
	clearPlayerTrace(state);
}

static void adjustLandPosition(GameState *state) {
	state->_landX = state->player.x;
	state->_landY = state->player.y;
}

static void prepareNextLevel(GameState *state) {
	reset(state, true);
}

static void checkWinCondition(GameState *state) {
	float landRatio = Playfield_computeTileFraction(&state->field, Tile_Land);

	if (landRatio < 0.75f)
		return;

	prepareNextLevel(state);
}

static void updatePlayer(GameState *state) {
	Player *player = &state->player;

	PlayerUpdateResult result = Player_update(player, &state->field);

	switch (result) {
		case PlayerUpdateResult_SeaMove:
			Playfield_setTile(&state->field, player->x, player->y, Tile_PlayerTrace);
			break;

		case PlayerUpdateResult_EnteredLand:
			fillTracedArea(state);
			checkWinCondition(state);
			break;

		case PlayerUpdateResult_Died:
			handlePlayerDeath(state);

		default:
			break;
	}

	if (result == PlayerUpdateResult_SeaMove)
		return;

	adjustLandPosition(state);
}

static void updateEnemies(GameState *state) {
	for (size_t nEnemy = 0; nEnemy < ARRAY_SIZE(state->enemies); nEnemy++) {
		EnemyUpdateResult result = Enemy_update(&state->enemies[nEnemy], &state->field);

		if (result == EnemyUpdateResult_TouchedTrace)
			Player_kill(&state->player);
	}
}

static void handleInput(GameState *state) {
	if (state->_inputHandled)
		return;

	Player_setDirection(&state->player, state->_inputState.direction);
	
	state->_inputHandled = true;
}

static void update(GameState *state) {
	Player *player = &state->player;

	assert(Player_isAlive(player));

	handleInput(state);

	// TODO: check this condition based on game settings
	if (Player_willTouchTraceNextUpdate(player, &state->field))
		Player_kill(player);

	updatePlayer(state);
	updateEnemies(state);
}

// TODO: handle situations when update itself is lagging
void GameState_update(GameState *this, float timeDelta) {
	const float tickDuration = 1.f / 20.f;

	while (this->_timeSinceLastUpdate >= tickDuration) {
		update(this);
		this->_timeSinceLastUpdate -= tickDuration;
	}

	this->_timeSinceLastUpdate += timeDelta;
}

void GameState_setInputState(GameState *this, const InputState *input) {
	this->_inputState = *input;
	this->_inputHandled = false;
}

void GameState_bakeDynamicObjects(GameState *this) {
	Playfield *field = &this->field;

	this->_tileUnderPlayer = Playfield_exchangeTile(field, this->player.x, this->player.y, Tile_PlayerHead);

	for (size_t nEnemy = 0; nEnemy < ARRAY_SIZE(this->enemies); nEnemy++) {
		const Enemy *enemy = &this->enemies[nEnemy];

		if (enemy->type == EnemyType_Disabled)
			continue;

		this->_tilesUnderEnemies[nEnemy] = Playfield_exchangeTile(
				field,
				enemy->x, enemy->y,
				(enemy->type == EnemyType_Sea) ? Tile_SeaEnemy : Tile_LandEnemy
		);
	}
}

void GameState_unbakeDynamicObjects(GameState *this) {
	Playfield *field = &this->field;

	for (int nEnemy = ARRAY_SIZE(this->enemies) - 1; nEnemy >= 0; nEnemy--) {
		const Enemy *enemy = &this->enemies[nEnemy];

		if (enemy->type == EnemyType_Disabled)
			continue;

		Playfield_setTile(field, enemy->x, enemy->y, this->_tilesUnderEnemies[nEnemy]);
	}

	Playfield_setTile(field, this->player.x, this->player.y, this->_tileUnderPlayer);
}