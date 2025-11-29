#pragma once
#include "DungeonContent.h"
#include "Player.h" 
#include "ConsoleControl_.h"

enum class LootType
{
    Potion,
    Coin,
    Weapon
};

class Loot : public DungeonContent
{
public:
    Loot(LootType type);

    LootType GetLootType() const { return _lootType; }

    bool OnPlayerCollect(Player& player);

private:
    LootType _lootType;

    void ApplyEffect(Player& player);
    static char GetSymbolFromLootType(LootType type);
};
