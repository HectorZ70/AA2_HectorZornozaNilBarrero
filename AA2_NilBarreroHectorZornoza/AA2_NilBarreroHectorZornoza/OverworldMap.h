#pragma once
#include "DungeonMap.h"
#include "InputSystem.h"
#include "Player.h"
#include "Enemy.h"
#include <map>

// El mapa grande de 3x3 que contiene los mapas pequeños
class OverworldMap
{
public:
	OverworldMap(Vector2 mapSize, Vector2 cellSize);
	~OverworldMap();

	void Run(InputSystem& input, Player& player);

private:
	// Mapa de los 9 mapas: (0,0) es la esquina superior izquierda.
	// El mapa inicial es (1, 1)
	DungeonMap* _dungeonMaps[3][3];
	Vector2 _currentMapIndex;
	Vector2 _playerPos;

	std::vector<Enemy*> _enemies;
	std::vector<std::thread> _enemyThreads;

	Vector2 _mapSize;
	Vector2 _cellSize;

	void HandleMovement(int key);
	void DrawCurrentMap();
	void ClampPlayerToMap();

	bool IsPortal(Vector2 pos);
	void ActivatePortal(Vector2 currentPos);
	bool IsChest(Vector2 pos);
	void DrawChest(Vector2 currentPos);
};