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

static void fillSea(Playfield *field, int x, int y) {
    if (Playfield_getTile(field, x, y) != Tile_Sea)
        return;

    Playfield_setTile(field, x, y, Tile_FillVisitedSea);

    fillSea(field, x, y + 1);
    fillSea(field, x, y - 1);
    fillSea(field, x - 1, y);
    fillSea(field, x + 1, y);
}

static void fill(GameState *state) {
    Playfield *field = &state->field;

    for (int nEnemy = 0; nEnemy < ARRAY_SIZE(state->enemies); nEnemy++) {
        const Enemy *enemy = &state->enemies[nEnemy];

        if (enemy->type != EnemyType_Sea)
            continue;

        fillSea(field, enemy->x, enemy->y);
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