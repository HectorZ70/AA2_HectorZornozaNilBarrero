#include "Loot.h"
#include <iostream>

char Loot::GetSymbolFromLootType(LootType type)
{
    switch (type)
    {
    case LootType::Potion: return '!'; 
    case LootType::Coin: return '$'; 
    case LootType::Weapon: return 'T'; 
    default: return '?';
    }
}

Loot::Loot(LootType type)
    : DungeonContent((TileType)type, GetSymbolFromLootType(type)), _lootType(type)
{
}

bool Loot::OnPlayerCollect(Player& player)
{
    ApplyEffect(player);
    return true; 
}

void Loot::ApplyEffect(Player& player)
{
    CC::Lock();
    CC::SetPosition(15, 1); 

    switch (_lootType)
    {
    case LootType::Potion:
        
        break;
    case LootType::Coin:
        
        break;
    case LootType::Weapon:

        break;
    }
    CC::Unlock();
}