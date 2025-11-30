#include "Chest.h"

Chest::Chest()
    : DungeonContent(TileType::Chest, 'C')
{
}

LootType Chest::GenerateRandomLootType()
{
    int random = rand() % 3;

    switch (random)
    {
    case 0: return LootType::Potion;
    case 1: return LootType::Coin;
    case 2: return LootType::Weapon;
    default: return LootType::Coin; 
    }
}

Loot* Chest::OpenChest()
{
    LootType loot = GenerateRandomLootType();

    return new Loot(loot);
}