#pragma once
#include <string>
#include "SaveState.h"

class SaveManager {
public:
    static bool SaveToFile(const SaveState& state, const std::string& filename = "autosave.json");
    static SaveState LoadFromFile(const std::string& filename = "autosave.json");
};