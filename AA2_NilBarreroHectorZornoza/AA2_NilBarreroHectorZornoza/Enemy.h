#pragma once
#include "Player.h"
#include "DungeonMap.h"
#include <mutex>

class Enemy 
{
private:
	int e_hp = 20;
	int dmg = 15;
	std::chrono::steady_clock::time_point _lastActionTime;
	std::chrono::milliseconds _coolDown{ 300 };
	std::mutex _mutexEnemy;
	Vector2 _pos;
	Vector2 _room;
	DungeonMap* _map;
	bool _isAttacking = false;
	bool running = true;
	bool _lootDropped = false;

public:
	Enemy(Vector2 initPos, Vector2 room, DungeonMap* map)
		: _pos(initPos), _room(room), _map(map) { }

	Vector2 GetPosition() const { return _pos; }
	Vector2 GetRoom() const { return _room; }

	bool HasDroppedLoot() const { return _lootDropped; }
	void MarkLootDropped() { _lootDropped = true; }

	void Stop() { running = false; }

	void MoveAI();

	void RunEnemies();

	void TakeDamage(int dmg);

	int GetDamage() const;
	
	bool IsDead() const;
};