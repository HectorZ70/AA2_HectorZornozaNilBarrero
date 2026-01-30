#include "SaveManager.h"
#include <json/json.h>
#include <fstream>
#include <iostream>

bool SaveManager::SaveToFile(const SaveState& state, const std::string& filename) {
    Json::Value root;

    root["player"]["hp"] = state.playerHP;
    root["player"]["potions"] = state.playerPotions;
    root["player"]["pos_x"] = state.playerPos.X;
    root["player"]["pos_y"] = state.playerPos.Y;

    root["world"]["map_x"] = state.currentMapIndex.X;
    root["world"]["map_y"] = state.currentMapIndex.Y;

    Json::Value enemies(Json::arrayValue);
    for (int i = 0; i < state.enemyCount; ++i) {
        Json::Value e;
        e["x"] = state.enemies[i].position.X;
        e["y"] = state.enemies[i].position.Y;
        e["hp"] = state.enemies[i].hp;
        e["dead"] = state.enemies[i].isDead;
        enemies.append(e);
    }
    root["enemies"] = enemies;

    try {
        std::ofstream file(filename);
        Json::StreamWriterBuilder builder;
        file << Json::writeString(builder, root);
        return true;
    }
    catch (...) {
        return false;
    }
}

SaveState SaveManager::LoadFromFile(const std::string& filename) {
    SaveState state;
    state.playerHP = 100;
    state.playerPotions = 4;
    state.enemyCount = 0;

    std::ifstream file(filename);
    if (!file.is_open()) {
        return state; 
    }

    Json::Value root;
    Json::CharReaderBuilder builder;
    std::string errs;

    if (!Json::parseFromStream(builder, file, &root, &errs)) {
        return state;
    }

    // --- CARGAR JUGADOR ---
    if (root.isMember("player")) {
        state.playerHP = root["player"]["hp"].asInt();
        state.playerPotions = root["player"]["potions"].asInt();
        state.playerPos.X = root["player"]["pos_x"].asInt();
        state.playerPos.Y = root["player"]["pos_y"].asInt();
    }

    // --- CARGAR MUNDO ---
    if (root.isMember("world")) {
        state.currentMapIndex.X = root["world"]["map_x"].asInt();
        state.currentMapIndex.Y = root["world"]["map_y"].asInt();
    }

    // --- CARGAR ENEMIGOS ---
    if (root.isMember("enemies") && root["enemies"].isArray()) {
        const Json::Value enemies = root["enemies"];
        state.enemyCount = (int)enemies.size();

        if (state.enemyCount > MAX_ENEMIES) state.enemyCount = MAX_ENEMIES;

        for (int i = 0; i < state.enemyCount; ++i) {
            state.enemies[i].position.X = enemies[i]["x"].asInt();
            state.enemies[i].position.Y = enemies[i]["y"].asInt();
            state.enemies[i].hp = enemies[i]["hp"].asInt();
            state.enemies[i].isDead = enemies[i]["dead"].asBool();
        }
    }

    file.close();
    return state;
}