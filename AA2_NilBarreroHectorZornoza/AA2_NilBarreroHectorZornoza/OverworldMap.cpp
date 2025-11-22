#include "OverworldMap.h"
#include "DungeonContent.h" // Necesario para el casting
#include <conio.h>          // Para _getch() en Windows (simulación)
#include "ConsoleControl_.h" 
#include "Player.h"

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
			_dungeonMaps[i][j] = new DungeonMap(_cellSize, offset);
		}
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
}

void OverworldMap::Run(InputSystem& input, Player& player)
{
	int key = 0;
	_playerPos = Vector2(_cellSize.X / 2, _cellSize.Y / 2);

	while (key != K_Q)
	{
		CC::Clear(); // Limpia la pantalla 
		DrawCurrentMap();

		// Dibuja un jugador simulado 'X' (no funka)
		Vector2 absolutePlayerPos = _playerPos + _dungeonMaps[_currentMapIndex.X][_currentMapIndex.Y]->GetNodeMap()->_offset;
		CC::Lock();
		CC::SetPosition(absolutePlayerPos.X, absolutePlayerPos.Y);
		std::cout << "X";

		CC::SetPosition(0, _cellSize.Y + 4);
		CC::Unlock();

		std::cout << "\n\n\n\n\n\n\n\n  (Q) Salir | (W/A/S/D) Mover | (E) Activar Portal ";

		key = _getch(); // Espera una tecla (simulación de entrada)

		if (key == K_E)
		{
			ActivatePortal(_playerPos);
		}
		else
		{
			HandleMovement(key);
		}
	}
	
}

void OverworldMap::DrawCurrentMap()
{
	_dungeonMaps[_currentMapIndex.X][_currentMapIndex.Y]->Draw();
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

	// Bloqueo seguro para verificar el nodo
	currentMap->SafePickNode(pos, [&](Node* node)
		{
			if (node == nullptr) return;

			// Utiliza el método template GetContent()
			DungeonContent* content = node->GetContent<DungeonContent>();

			if (content != nullptr && content->GetType() == TileType::Portal)
			{
				isPortal = true;
			}
		});

	return isPortal;
}

void OverworldMap::ActivatePortal(Vector2 currentPos)
{
	if (!IsPortal(currentPos))
	{
		return;
	}

	// Lógica de cambio de mapa
	// Comprobar qué lado del mapa estamos 
	if (currentPos.X == 0) // Portal Izquierda
	{
		if (_currentMapIndex.X > 0) _currentMapIndex.X--;
	}
	else if (currentPos.X == _cellSize.X - 1) // Portal Derecha
	{
		if (_currentMapIndex.X < 2) _currentMapIndex.X++;
	}
	else if (currentPos.Y == 0) // Portal Arriba
	{
		if (_currentMapIndex.Y > 0) _currentMapIndex.Y--;
	}
	else if (currentPos.Y == _cellSize.Y - 1) // Portal Abajo
	{
		if (_currentMapIndex.Y < 2) _currentMapIndex.Y++;
	}
}

bool OverworldMap::IsChest(Vector2 pos)
{
	NodeMap* currentMap = _dungeonMaps[_currentMapIndex.X][_currentMapIndex.Y]->GetNodeMap();
	bool isPortal = false;

	// Bloqueo seguro para verificar el nodo
	currentMap->SafePickNode(pos, [&](Node* node)
		{
			if (node == nullptr) return;

			// Utiliza el método template GetContent()
			DungeonContent* content = node->GetContent<DungeonContent>();

			if (content != nullptr && content->GetType() == TileType::Chest)
			{
				isPortal = true;
			}
		});

	return isPortal;
}

void OverworldMap::DrawChest(Vector2 currentPos)
{
	if (!IsChest(currentPos))
		return;

	if (currentPos.X == 1) // Portal Izquierda
	{
		if (_currentMapIndex.X > 0) _currentMapIndex.X--;
	}
	else if (currentPos.X == _cellSize.X + 1) // Portal Derecha
	{
		if (_currentMapIndex.X < 2) _currentMapIndex.X++;
	}
	else if (currentPos.Y == 1) // Portal Arriba
	{
		if (_currentMapIndex.Y > 0) _currentMapIndex.Y--;
	}
	else if (currentPos.Y == _cellSize.Y + 1) // Portal Abajo
	{
		if (_currentMapIndex.Y < 2) _currentMapIndex.Y++;
	}
}
