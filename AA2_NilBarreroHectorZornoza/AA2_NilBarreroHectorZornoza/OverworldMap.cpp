#include "OverworldMap.h"
#include "DungeonContent.h" // Necesario para el casting
#include <conio.h>          // Para _getch() en Windows (simulación)
#include "ConsoleControl_.h" 

using CC = ConsoleControl;

OverworldMap::OverworldMap(Vector2 mapSize, Vector2 cellSize)
	: _mapSize(mapSize), _cellSize(cellSize), _currentMapIndex(1, 1), _playerPos(0, 0) // Inicia en el centro

{
	// Inicializa los 9 mapas
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			// Cálculo del offset: cada mapa tiene el tamaño _cellSize.
			Vector2 offset(i * _cellSize.X, j * _cellSize.Y);
			_dungeonMaps[i][j] = new DungeonMap(Vector2(i, j), _cellSize, offset);
		}
	}

	_enemies.push_back(
		new Enemy(Vector2(4, 5), Vector2(1, 1), _dungeonMaps[1][1])
	);

	_enemies.push_back(
		new Enemy(Vector2(8, 2), Vector2(1, 1), _dungeonMaps[1][1])
	);

	for (Enemy* e : _enemies)
	{
		_enemyThreads.emplace_back(&Enemy::RunEnemies, e);
	}
}

OverworldMap::~OverworldMap()
{
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			delete _dungeonMaps[i][j];
		}
	}

	for (Enemy* e : _enemies)
		e->Stop();

	for (std::thread& t : _enemyThreads)
		t.join();

	for (Enemy* e : _enemies)
		delete e;
}

void OverworldMap::Run(InputSystem& input, Player& player)
{
	int key = 0;
	_playerPos = Vector2(_cellSize.X / 2, _cellSize.Y / 2);
	player.SetPosition(_playerPos);

	while (key != K_ESCAPE)
	{
		CC::Clear(); // Limpia la pantalla 
		DrawCurrentMap();

		Vector2 currentMapOffset = _dungeonMaps[_currentMapIndex.X][_currentMapIndex.Y]->GetNodeMap()->_offset;
		Vector2 playerPosInMap = player.GetPosition();

		CC::Lock();
		CC::SetPosition(playerPosInMap.X + currentMapOffset.X, playerPosInMap.Y + currentMapOffset.Y);
		CC::SetColor(CC::GREEN, CC::BLACK);
		std::cout << "J";

		CC::SetPosition(0, _cellSize.Y + 4 + currentMapOffset.Y);
		CC::SetColor(CC::WHITE, CC::BLACK);
		std::cout << "Mapa: [" << _currentMapIndex.X << ", " << _currentMapIndex.Y << "] | Pos: [" << playerPosInMap.X << ", " << playerPosInMap.Y << "]";
		std::cout << "\n\n(ESC) Salir | (W/A/S/D) Mover ";
		CC::Unlock();

		key = _getch();

		if (key == K_W || key == K_S || key == K_A || key == K_D)
		{
			Vector2 oldPos = player.GetPosition();

			player.Move(key);

			Vector2 newPos = player.GetPosition();

			if (newPos.X != oldPos.X || newPos.Y != oldPos.Y)
			{
				if (IsPortal(newPos))
				{
					ActivatePortal(newPos, player);
				}
				else if (IsWall(newPos))
				{
					player.SetPosition(oldPos);
				}
				// Interacción con Cofre (si se implementa)
				else if (IsChest(newPos))
				{

				}

				_playerPos = player.GetPosition();
			}
		}
	}
	
}

void OverworldMap::DrawCurrentMap()
{
	DungeonMap* map = _dungeonMaps[_currentMapIndex.X][_currentMapIndex.Y];
	map->Draw();

	Vector2 offset = map->GetNodeMap()->_offset;

	for (Enemy* e : _enemies)
	{
		Vector2 room = e->GetRoom();

		// Si el enemigo está en el mismo mapa que el jugador, se muestra
		if (room.X == _currentMapIndex.X && room.Y == _currentMapIndex.Y)
		{
			Vector2 pos = e->GetPosition();
			Vector2 absPos = pos + offset;

			CC::SetPosition(absPos.X, absPos.Y);
			CC::SetColor(CC::RED, CC::BLACK);
			std::cout << "E";
		}
	}
}

void OverworldMap::HandleMovement(int key)
{
	switch (key)
	{
	case K_W: _playerPos.Y--; break;
	case K_S: _playerPos.Y++; break;
	case K_A: _playerPos.X--; break;
	case K_D: _playerPos.X++; break;
	}

	ClampPlayerToMap();
}

void OverworldMap::ClampPlayerToMap()
{
	if (_playerPos.X < 0) _playerPos.X = 0;
	if (_playerPos.Y < 0) _playerPos.Y = 0;

	int maxX = _cellSize.X - 1;
	int maxY = _cellSize.Y - 1;

	if (_playerPos.X > maxX) _playerPos.X = maxX;
	if (_playerPos.Y > maxY) _playerPos.Y = maxY;
}

bool OverworldMap::IsPortal(Vector2 pos)
{
	NodeMap* currentMap = _dungeonMaps[_currentMapIndex.X][_currentMapIndex.Y]->GetNodeMap();
	bool isPortal = false;

	currentMap->SafePickNode(pos, [&](Node* node)
		{
			if (node == nullptr) return;

			DungeonContent* content = node->GetContent<DungeonContent>();

			if (content != nullptr && content->GetType() == TileType::Portal)
			{
				isPortal = true;
			}
		});

	return isPortal;
}

void OverworldMap::ActivatePortal(Vector2 currentPos, Player& player)
{
	int prevMapX = _currentMapIndex.X;
	int prevMapY = _currentMapIndex.Y;
	Vector2 newPos = currentPos;
	bool mapChanged = false;

	int maxMapIndexX = _mapSize.X - 1;
	int maxMapIndexY = _mapSize.Y - 1;

	if (currentPos.X == 0) // Portal Izquierda 
	{
		if (_currentMapIndex.X > 0)
		{
			_currentMapIndex.X--;
			newPos.X = _cellSize.X - 2;
			mapChanged = true;
		}
	}
	else if (currentPos.X == _cellSize.X - 1) // Portal Derecha 
	{
		if (_currentMapIndex.X < maxMapIndexX)
		{
			_currentMapIndex.X++;
			newPos.X = 1;
			mapChanged = true;
		}
	}
	else if (currentPos.Y == 0) // Portal Arriba 
	{
		if (_currentMapIndex.Y > 0)
		{
			_currentMapIndex.Y--;
			newPos.Y = _cellSize.Y - 2;
			mapChanged = true;
		}
	}
	else if (currentPos.Y == _cellSize.Y - 1) // Portal Abajo 
	{
		if (_currentMapIndex.Y < maxMapIndexY)
		{
			_currentMapIndex.Y++;
			newPos.Y = 1;
			mapChanged = true;
		}
	}

	// Si el índice del mapa ha cambiado, actualiza la posición del jugador y limpia la consola
	if (mapChanged)
	{
		_playerPos = newPos;

		player.SetPosition(_playerPos);
	}
}

bool OverworldMap::IsChest(Vector2 pos)
{
	NodeMap* currentMap = _dungeonMaps[_currentMapIndex.Y][_currentMapIndex.X]->GetNodeMap();
	bool isChest = false;

	// Bloqueo seguro para verificar el nodo
	currentMap->SafePickNode(pos, [&](Node* node)
		{
			if (node == nullptr) return;

			// Utiliza el método template GetContent()
			DungeonContent* content = node->GetContent<DungeonContent>();

			if (content != nullptr && content->GetType() == TileType::Chest)
			{
				isChest = true;
			}
		});

	return isChest;
}

bool OverworldMap::IsWall(Vector2 pos)
{
	NodeMap* currentMap = _dungeonMaps[_currentMapIndex.Y][_currentMapIndex.X]->GetNodeMap();
	bool isWall = false;

	// Bloqueo seguro para verificar el nodo
	currentMap->SafePickNode(pos, [&](Node* node)
		{
			if (node == nullptr) return;

			// Utiliza el método template GetContent()
			DungeonContent* content = node->GetContent<DungeonContent>();

			if (content != nullptr && content->GetType() == TileType::Wall)
			{
				isWall = true;
			}
		});

	return isWall;
}


