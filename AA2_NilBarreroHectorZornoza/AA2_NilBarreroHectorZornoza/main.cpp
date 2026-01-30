#include <iostream>
#include <thread>
#include <atomic>
#include "OverworldMap.h"
#include "ConsoleControl_.h"
#include "SaveManager.h"
#include "InputSystem.h"

using namespace std;

atomic<bool> isGameRunning(true);

void AutoSaveLoop(OverworldMap& gameMap, Player& player) {
    while (isGameRunning) {
        this_thread::sleep_for(chrono::seconds(5));

        if (!isGameRunning) break;

        SaveState current = gameMap.CaptureCurrentState(player);
        SaveManager::SaveToFile(current, "autosave.json");
    }
}

int main() {
    srand((time(NULL)));

    Vector2 cellSize(10, 10);
    Vector2 mapSize(3, 3);

    Player player;
    OverworldMap gameMap(mapSize, cellSize);
    InputSystem input;

    SaveState loadedData = SaveManager::LoadFromFile("autosave.json");

    if (loadedData.playerHP > 0) {
        player.SetHP(loadedData.playerHP);
        player.SetPosition(loadedData.playerPos);
        player.SetPotions(loadedData.playerPotions);
        gameMap.SetCurrentMap(loadedData.currentMapIndex);

        CC::Lock();
        cout << "Partida cargada exitosamente." << endl;
        this_thread::sleep_for(chrono::milliseconds(500));
        CC::Unlock();
    }
    else {
        player.SetPosition(Vector2(5, 5));
        gameMap.SetCurrentMap(Vector2(1, 1));
    }

    thread saveThread(AutoSaveLoop, ref(gameMap), ref(player));

    gameMap.Run(input, player);

    isGameRunning = false;
    if (saveThread.joinable()) {
        saveThread.join();
    }

    CC::Lock();
    CC::Clear();
    cout << "Juego cerrado. Partida guardada." << endl;
    CC::Unlock();

    return 0;
}