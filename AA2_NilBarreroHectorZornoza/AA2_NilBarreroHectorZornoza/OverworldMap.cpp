#include "OverworldMap.h"
#include "DungeonContent.h" // Necesario para el casting
#include <conio.h>          // Para _getch() en Windows (simulacion)
#include "ConsoleControl_.h" 
#include "Chest.h"
#include "Loot.h"
#include "Enemy.h"

using CC = ConsoleControl;

OverworldMap::OverworldMap(Vector2 mapSize, Vector2 cellSize)
	: _mapSize(mapSize), _cellSize(cellSize), _currentMapIndex(1, 1), _playerPos(0, 0) // Inicia en el centro

{
	// Inicializa los 9 mapas
	for (int i = 0; i < _mapSize.X; ++i)
	{
		for (int j = 0; j < _mapSize.Y; ++j)
		{
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
	for (int i = 0; i < _mapSize.X; ++i)
	{
		for (int j = 0; j < _mapSize.Y; ++j)
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
	_playerPos = Vector2(_cellSize.X / 2, _cellSize.Y / 2);
	player.SetPosition(_playerPos);
	bool running = true;

	input.AddListener(K_W, [&]() {
		Vector2 next = player.GetPosition() + Vector2(0, -1);
		if (!IsWall(next))
			player.Move(K_W);
		});

	input.AddListener(K_S, [&]() {
		Vector2 next = player.GetPosition() + Vector2(0, 1);
		if (!IsWall(next))
			player.Move(K_S);
		});

	input.AddListener(K_A, [&]() {
		Vector2 next = player.GetPosition() + Vector2(-1, 0);
		if (!IsWall(next))
			player.Move(K_A);
		});

	input.AddListener(K_D, [&]() {
		Vector2 next = player.GetPosition() + Vector2(1, 0);
		if (!IsWall(next))
			player.Move(K_D);
		});
	input.AddListener(K_E, [&]() {
		if (IsChest(player.GetPosition())) {
			InteractWithChest(player);
		}
		});

	input.AddListener(K_E, [&]() {
		Enemy* enemy = GetEnemyAt(player.GetPosition());
		if (enemy && !enemy->IsDead())
		{
			enemy->TakeDamage(player.GetDamage());
		}
		});
	input.AddListener(K_Q, [&]() { player.DrinkPoption(K_Q); });

	input.AddListener(K_ESCAPE, [&]() { running = false; });

	input.StartListen();

	SpawnChest(1);
	SpawnCoins(10);
	
	while (running)
	{
		CC::Clear(); 
		
		UpdateEnemies();
		DrawCurrentMap();
		DrawHUD(player);
		

		Vector2 currentMapOffset = _dungeonMaps[_currentMapIndex.X][_currentMapIndex.Y]->GetNodeMap()->_offset;
		Vector2 playerPosInMap = player.GetPosition();
		Enemy* enemy = GetEnemyAt(player.GetPosition());

		if (IsPortal(playerPosInMap)) {
			ActivatePortal(playerPosInMap, player);
		}

		TryPickCoin(player);

		if (enemy != nullptr)
		{
			player.TakeDamage(enemy->GetDamage());
			
			//if (player.IsDead())
				//running = false;
		}

		CC::Lock();
		CC::SetPosition(playerPosInMap.X + currentMapOffset.X, playerPosInMap.Y + currentMapOffset.Y);
		//player.PrintPosition();
		CC::SetColor(CC::GREEN, CC::BLACK);
		std::cout << "J";
		
		CC::SetPosition(0, _cellSize.Y + 4 + currentMapOffset.Y);
		CC::SetColor(CC::WHITE, CC::BLACK);
		CC::Unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(8));
	}

	input.StopListen();
}

void OverworldMap::SpawnCoins(int count)
{
	NodeMap* map =
		_dungeonMaps[_currentMapIndex.X][_currentMapIndex.Y]->GetNodeMap();

	int spawned = 0;
	int attempts = 0;
	const int MAX_ATTEMPTS = 100;

	while (spawned < count && attempts < MAX_ATTEMPTS)
	{
		int x = rand() % _cellSize.X;
		int y = rand() % _cellSize.Y;

		map->SafePickNode(Vector2(x, y), [&](Node* node)
			{
				if (!node) return;

				DungeonContent* content = node->GetContent<DungeonContent>();
				if (content && content->GetType() == TileType::Empty)
				{
					node->SetContent(new DungeonContent(TileType::Coins, '$'));
					spawned++;
				}
			});

		attempts++;
	}
}

void OverworldMap::TryPickCoin(Player& player)
{
	NodeMap* map =
		_dungeonMaps[_currentMapIndex.X][_currentMapIndex.Y]->GetNodeMap();

	map->SafePickNode(player.GetPosition(), [&](Node* node)
		{
			if (!node) return;

			DungeonContent* content = node->GetContent<DungeonContent>();
			if (!content) return;

			if (content->GetType() == TileType::Coins)
			{
				player.AddCoins(1); // o 5, 10, lo que quieras
				node->SetContent(new DungeonContent(TileType::Empty, ' '));
			}
		});
}

void OverworldMap::SpawnChest(int count)
{
	NodeMap* map = _dungeonMaps[_currentMapIndex.X][_currentMapIndex.Y]->GetNodeMap();

	int spawned = 0;
	int attempts = 0;
	const int MAX_ATTEMPTS = 100;

	while (spawned < count && attempts < MAX_ATTEMPTS)
	{
		int x = rand() % _cellSize.X;
		int y = rand() % _cellSize.Y;

		map->SafePickNode(Vector2(x, y), [&](Node* node)
			{
				if (!node) return;

				DungeonContent* content = node->GetContent<DungeonContent>();
				if (content && content->GetType() == TileType::Empty)
				{
					node->SetContent(new Chest());
					spawned++;
				}
			});

		attempts++;
	}
}

void OverworldMap::InteractWithChest(Player& player)
{
	NodeMap* currentMap =
		_dungeonMaps[_currentMapIndex.X][_currentMapIndex.Y]->GetNodeMap();


	currentMap->SafePickNode(player.GetPosition(), [&](Node* node)
		{
			if (!node) return;

			Chest* chest = node->GetContent<Chest>();
			if (!chest) return;

			Loot* loot = chest->OpenChest();

			player.AddCoins(100);
			player.AddPotion();
			switch (loot->GetLootType())
			{
			case LootType::Potion:
				player.AddPotion();
				break;

			case LootType::Coin:
				player.AddCoins(100);
				break;

			case LootType::Weapon:
				// EquipWeapon
				break;
			}

			node->SetContent(new DungeonContent(TileType::Empty, ' '));
			delete loot;
		});
}

void OverworldMap::DrawCurrentMap()
{
	DungeonMap* map = _dungeonMaps[_currentMapIndex.X][_currentMapIndex.Y];
	map->Draw();

	NodeMap* nodeMap = map->GetNodeMap();
	Vector2 offset = nodeMap->_offset;

	CC::Lock();

	for (int y = 0; y < _cellSize.Y; ++y)
	{
		for (int x = 0; x < _cellSize.X; ++x)
		{
			Vector2 pos(x, y);
			nodeMap->SafePickNode(pos, [&](Node* node)
				{
					if (!node) return;

					DungeonContent* content = node->GetContent<DungeonContent>();
					if (!content) return;

					Vector2 absPos = pos + offset;
					content->Draw(absPos);
				});
		}
	}

	for (Enemy* e : _enemies) 
	{ 
		Vector2 room = e->GetRoom(); 
		if (room.X == _currentMapIndex.X && room.Y == _currentMapIndex.Y) 
		{ 
			Vector2 absPos = e->GetPosition() + offset; 
			CC::SetPosition(absPos.X, absPos.Y); 
			CC::SetColor(CC::RED, CC::BLACK); 
			std::cout << "E"; 
		} 
	}

	CC::Unlock();
}

void OverworldMap::DrawHUD(Player& player)
{
	int hudX = _cellSize.X + 30;
	int hudY = _cellSize.Y;

	CC::SetColor(CC::WHITE, CC::BLACK);

	CC::SetPosition(hudX, hudY);
	std::cout << "HP: " << player.GetHP();

	CC::SetPosition(hudX, hudY + 1);
	std::cout << "Pots: " << player.GetPotions();

	CC::SetPosition(hudX, hudY + 2);
	std::cout << "Coins: " << player.GetCoins();
}

void OverworldMap::UpdateEnemies()
{
	for (Enemy* enemy : _enemies)
	{
		enemy->MoveAI();
	}
}

bool OverworldMap::IsPortal(Vector2 pos)
{
	NodeMap* currentMap = _dungeonMaps[_currentMapIndex.X][_currentMapIndex.Y]->GetNodeMap();
	bool isPortal = false;

	currentMap->SafePickNode(pos, [&](Node* node)
		{
			if (!node) return;

			// Intentamos hacer cast seguro
			DungeonContent* content = dynamic_cast<DungeonContent*>(node->GetContent<INodeContent>());
			if (!content) return; // Si no es DungeonContent, salimos

			if (content->GetType() == TileType::Portal)
			{
				isPortal = true;
			}
		});

	return isPortal;
}

Enemy* OverworldMap::GetEnemyAt(const Vector2& pos)
{
	for (auto& e : _enemies)
		if (e->GetPosition().X == pos.X &&
			e->GetPosition().Y == pos.Y)
			return e;

	return nullptr;
}

void OverworldMap::ActivatePortal(Vector2 currentPos, Player& player)
{
	int prevMapX = _currentMapIndex.X;
	int prevMapY = _currentMapIndex.Y;
	Vector2 newPos = currentPos; // portal pos
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

	if (mapChanged)
	{
		_playerPos = newPos;

		player.SetPosition(_playerPos);
	}
}

bool OverworldMap::IsChest(Vector2 pos)
{
	NodeMap* currentMap = _dungeonMaps[_currentMapIndex.X][_currentMapIndex.Y]->GetNodeMap();
	bool isChest = false;

	currentMap->SafePickNode(pos, [&](Node* node)
		{
			if (node == nullptr) return;

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
	NodeMap* currentMap = _dungeonMaps[_currentMapIndex.X][_currentMapIndex.Y]->GetNodeMap();
	bool isWall = false;

	currentMap->SafePickNode(pos, [&](Node* node)
		{
			if (node == nullptr) return;

			DungeonContent* content = node->GetContent<DungeonContent>();

			if (content != nullptr && content->GetType() == TileType::Wall)
			{
				isWall = true;
			}
		});

	return isWall;
}

SaveState OverworldMap::CaptureCurrentState(Player& player)
{
	SaveState state;

	state.playerHP = player.GetHP();
	state.playerPotions = player.GetPotions();
	state.playerPos = player.GetPosition(); 

	state.currentMapIndex = _currentMapIndex;

	state.enemyCount = (int)_enemies.size();
	if (state.enemyCount > MAX_ENEMIES) state.enemyCount = MAX_ENEMIES;

	for (int i = 0; i < state.enemyCount; ++i)
	{
		state.enemies[i].position = _enemies[i]->GetPosition();
		state.enemies[i].room = _enemies[i]->GetRoom();
		state.enemies[i].isDead = _enemies[i]->IsDead();
		state.enemies[i].hp = 20;
	}

	return state;
}

void OverworldMap::SetCurrentMap(Vector2 mapIndex)
{
	if (mapIndex.X >= 0 && mapIndex.X < 3 && mapIndex.Y >= 0 && mapIndex.Y < 3)
	{
		_currentMapIndex = mapIndex;
	}
}
