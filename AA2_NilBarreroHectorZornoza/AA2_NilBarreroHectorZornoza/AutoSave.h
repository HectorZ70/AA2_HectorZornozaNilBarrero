#pragma once
#include <thread>
#include <atomic>
#include "OverworldMap.h"
#include "SaveState.h"
#include "SaveManager.h" 

class AutoSaveSystem {
private:
    std::atomic<bool> _running{ false };
    std::thread _saveThread;

public:
    ~AutoSaveSystem() {
        Stop();
    }

    void Start(OverworldMap& gameMap, Player& player) {
        if (_running) return; 

        _running = true;
        _saveThread = std::thread([this, &gameMap, &player]() {
            while (_running) {
                for (int i = 0; i < 50 && _running; ++i) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }

                if (!_running) break;

                SaveState currentState = gameMap.CaptureCurrentState(player);

                SaveManager::SaveToFile(currentState, "autosave.json");
            }
            });
    }

    void Stop() {
        _running = false;
        if (_saveThread.joinable()) {
            _saveThread.join(); 
        }
    }
};