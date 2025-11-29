#pragma once
#include "DungeonContent.h"
#include "Loot.h"
#include <cstdlib>
#include <ctime>
#include <memory> 

class Chest : public DungeonContent
{
public:
    Chest();

    Loot* OpenChest();

private:
    LootType GenerateRandomLootType();
};