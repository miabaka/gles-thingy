#include "game_state.h"

#ifdef _WIN32
#include <malloc.h>
#else
#include <alloca.h>
#endif

#include <string.h>

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

static void fillSea(Playfield *field, int x, int y) {
    if (Playfield_getTile(field, x, y) != Tile_Sea)
        return;

    Playfield_setTile(field, x, y, Tile_Land);

    fillSea(field, x, y + 1);
    fillSea(field, x, y - 1);
    fillSea(field, x - 1, y);
    fillSea(field, x + 1, y);
}

static void fill(GameState *state) {
    Playfield *field = &state->field;
    size_t fieldSizeBytes = Playfield_getSizeBytes(field);

    Playfield inverseField = *field;

    inverseField.tiles = (Tile *) alloca(fieldSizeBytes);

    if (!inverseField.tiles)
        return;

    memcpy(inverseField.tiles, field->tiles, fieldSizeBytes);

    for (int nEnemy = 0; nEnemy < ARRAY_SIZE(state->enemies); nEnemy++) {
        const Enemy *enemy = &state->enemies[nEnemy];

        if (enemy->type != EnemyType_Sea)
            continue;

        fillSea(&inverseField, enemy->x, enemy->y);
    }

    for (int nTile = 0; nTile < Playfield_getSizeTiles(field); nTile++) {
        Tile inverseTile = inverseField.tiles[nTile];

        if (!(inverseTile == Tile_Sea || inverseTile == Tile_PlayerTrace))
            continue;

        field->tiles[nTile] = Tile_Land;
    }
}

static void update(GameState *state) {
    PlayerUpdateResult playerUpdateResult = Player_update(&state->player, &state->field);

    if (playerUpdateResult != PlayerUpdateResult_TraceEnded)
        return;

    fill(state);
}

void GameState_update(GameState *this, float timeDelta) {
    if (this->_timeSinceLastUpdate > 0.05f) {
        update(this);
        this->_timeSinceLastUpdate = 0.f;
    }

    this->_timeSinceLastUpdate += timeDelta;
}

void GameState_applyInputState(GameState *this, const InputState *input) {
    Player_setDirection(&this->player, input->direction);
}