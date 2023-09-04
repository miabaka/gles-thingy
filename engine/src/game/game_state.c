#include "game_state.h"

#include "../utils/array.h"

bool GameState_init(GameState *this, uint8_t fieldWidth, uint8_t fieldHeight) {
    if (!Playfield_init(&this->field, fieldWidth, fieldHeight))
        return false;

    Player_init(&this->player);

    for (int nEnemy = 0; nEnemy < ARRAY_SIZE(this->enemies); nEnemy++)
        Enemy_init(&this->enemies[nEnemy]);

    return true;
}

void GameState_destroy(GameState *this) {
    Playfield_destroy(&this->field);
}