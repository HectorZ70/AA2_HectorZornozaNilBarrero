#pragma once
#include "NodeMap_.h"

// Un mapa pequeño (una "sala" de la mazmorra)
class DungeonMap
{
public:
	DungeonMap(Vector2 size, Vector2 offset);
	~DungeonMap();

	void Draw();

	NodeMap* GetNodeMap();

private:
	NodeMap* _map;

	void SetupMap(Vector2 size);
};