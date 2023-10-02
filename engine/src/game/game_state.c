#include "game_state.h"

#include <assert.h>
#include "../utils/common.h"

static void resetPlayer(GameState *state) {
	Player *player = &state->player;
	Player_reset(player);
	Player_setPosition(player, state->_landPosition);
}

static void reset(GameState *state, bool resetField) {
	if (resetField)
		Playfield_reset(&state->field);

	state->_landPosition = (ivec2) {state->field.width / 2, 0};
	state->_timeSinceLastUpdate = 0.f;

	resetPlayer(state);

	for (size_t nEnemy = 0; nEnemy < ARRAY_SIZE(state->enemies); nEnemy++)
		// Actually, multiple initializations is fine because enemies don't allocate
		// any dynamic memory so it acts like an reset
		// TODO: separate reset and init routines
		Enemy_init(&state->enemies[nEnemy]);

	state->enemies[0].type = EnemyType_Sea;
	state->enemies[0].position = (ivec2) {10, 10};

	state->enemies[1].type = EnemyType_Sea;
	state->enemies[1].position = (ivec2) {70, 40};
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

		Playfield_fill(field, enemy->position, Tile_FillVisitedSea);
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
	Player_setPosition(player, state->_landPosition);
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
	state->_landPosition = state->player.position;
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
			Playfield_setTile(&state->field, player->position, Tile_PlayerTrace);
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
	for (size_t nEnemy = 0; nEnemy < ARRAY_SIZE(state->enemies); nEnemy++)
		Enemy_update(&state->enemies[nEnemy], &state->field);
}

static void handleInput(GameState *state) {
	if (state->_inputHandled)
		return;

	Player_setDirection(&state->player, state->_inputState.direction);

	state->_inputHandled = true;
}

static bool isPlayerCollidingWithEnemies(const GameState *state) {
	static const ivec2 sideOffsets[] = {
			{ 0, -1},
			{-1,  0},
			{ 1,  0},
			{ 0,  1}
	};

	const Player *player = &state->player;

	if (player->state != PlayerState_SeaMoving)
		return false;

	for (size_t nEnemy = 0; nEnemy < ARRAY_SIZE(state->enemies); nEnemy++) {
		const Enemy *enemy = &state->enemies[nEnemy];

		if (enemy->type == EnemyType_Disabled)
			continue;

		for (size_t nSide = 0; nSide < ARRAY_SIZE(sideOffsets); nSide++) {
			ivec2 p = ivec2_add(enemy->position, sideOffsets[nSide]);

			Tile tile = Playfield_getTile(&state->field, p);

			if (tile == Tile_PlayerTrace || (p.x == player->position.x && p.y == player->position.y))
				return true;
		}
	}

	return false;
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

	if (isPlayerCollidingWithEnemies(state))
		Player_kill(player);
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

	this->_tileUnderPlayer = Playfield_exchangeTile(field, this->player.position, Tile_PlayerHead);

	for (size_t nEnemy = 0; nEnemy < ARRAY_SIZE(this->enemies); nEnemy++) {
		const Enemy *enemy = &this->enemies[nEnemy];

		if (enemy->type == EnemyType_Disabled)
			continue;

		this->_tilesUnderEnemies[nEnemy] = Playfield_exchangeTile(
				field, enemy->position,
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

		Playfield_setTile(field, enemy->position, this->_tilesUnderEnemies[nEnemy]);
	}

	Playfield_setTile(field, this->player.position, this->_tileUnderPlayer);
}