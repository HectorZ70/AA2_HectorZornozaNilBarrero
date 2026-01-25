#pragma once
#include "Vector2_.h"
#define MAX_ENEMIES 50

struct EnemySaveData {
    Vector2 position;
    Vector2 room;
    int hp;
    bool isDead;
};

struct SaveState {
    int playerHP;
    int playerPotions;
    Vector2 playerPos;

    Vector2 currentMapIndex; 

    int enemyCount;
    EnemySaveData enemies[MAX_ENEMIES];
};