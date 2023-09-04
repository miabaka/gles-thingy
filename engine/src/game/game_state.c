#include "game_state.h"

#include "../utils/array.h"

bool GameState_init(GameState *this, uint8_t fieldWidth, uint8_t fieldHeight) {
    if (!Playfield_init(&this->field, fieldWidth, fieldHeight))
        return false;

    Player_init(&this->player);

    for (int nEnemy = 0; nEnemy < ARRAY_SIZE(this->enemies); nEnemy++)
        Enemy_init(&this->enemies[nEnemy]);

    this->_timeSinceLastUpdate = 0.f;

    return true;
}

void GameState_destroy(GameState *this) {
    Playfield_destroy(&this->field);
}

static void update(GameState *state) {
    Player_update(&state->player, &state->field);
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