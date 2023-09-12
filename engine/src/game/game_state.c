#include "game_state.h"

#include "../utils/array.h"

bool GameState_init(GameState *this, uint8_t fieldWidth, uint8_t fieldHeight) {
	if (!Playfield_init(&this->field, fieldWidth, fieldHeight))
		return false;

	Player_init(&this->player);

	for (int nEnemy = 0; nEnemy < ARRAY_SIZE(this->enemies); nEnemy++)
		Enemy_init(&this->enemies[nEnemy]);

	this->enemies[0].type = EnemyType_Sea;
	this->enemies[0].x = 10;
	this->enemies[0].y = 10;

	this->enemies[1].type = EnemyType_Sea;
	this->enemies[1].x = 70;
	this->enemies[1].y = 40;

	this->_timeSinceLastUpdate = 0.f;

	return true;
}

void GameState_destroy(GameState *this) {
	Playfield_destroy(&this->field);
}

static void fillTracedArea(GameState *state) {
	Playfield *field = &state->field;

	for (int nEnemy = 0; nEnemy < ARRAY_SIZE(state->enemies); nEnemy++) {
		const Enemy *enemy = &state->enemies[nEnemy];

		if (enemy->type != EnemyType_Sea)
			continue;

		Playfield_fill(field, enemy->x, enemy->y, Tile_FillVisitedSea);
	}

	for (int nTile = 0; nTile < Playfield_getSizeTiles(field); nTile++) {
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

static void updatePlayer(GameState *state) {
	Player *player = &state->player;

	PlayerUpdateResult result = Player_update(player, &state->field);

	switch (result) {
		case PlayerUpdateResult_SeaMove:
			Playfield_setTile(&state->field, player->x, player->y, Tile_PlayerTrace);
			break;

		case PlayerUpdateResult_EnteredLand:
			fillTracedArea(state);

		default:
			break;
	}
}

static bool updateEnemies(GameState *state) {
	bool touchedTrace = false;

	for (int nEnemy = 0; nEnemy < ARRAY_SIZE(state->enemies); nEnemy++) {
		EnemyUpdateResult result = Enemy_update(&state->enemies[nEnemy], &state->field);
		touchedTrace |= (result == EnemyUpdateResult_TouchedTrace);
	}

	return touchedTrace;
}

static void update(GameState *state) {
	if (!Player_isAlive(&state->player))
		return;

	updatePlayer(state);

	if (updateEnemies(state))
		Player_kill(&state->player);
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

void GameState_applyInputState(GameState *this, const InputState *input) {
	Player_setDirection(&this->player, input->direction);
}

void GameState_bakeDynamicObjects(GameState *this) {
	Playfield *field = &this->field;

	this->_tileUnderPlayer = Playfield_exchangeTile(field, this->player.x, this->player.y, Tile_PlayerHead);

	for (int nEnemy = 0; nEnemy < ARRAY_SIZE(this->enemies); nEnemy++) {
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