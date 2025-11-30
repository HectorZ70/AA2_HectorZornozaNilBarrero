#pragma once
#include "Player.h"
#include "DungeonMap.h"
#include <mutex>

class Enemy 
{
private:
	int e_hp = 20;
	Vector2 _pos;
	Vector2 _room;
	DungeonMap* _map;
	bool running = true;

public:
	Enemy(Vector2 initPos, Vector2 room, DungeonMap* map)
		: _pos(initPos), _room(room), _map(map) { }

	Vector2 GetPosition() const { return _pos; }
	Vector2 GetRoom() const { return _room; }

	void Stop() { running = false; }

	void RunEnemies()
	{
		while (running)
		{
			MoveAI();
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	}
	void MoveAI();

	void TakeDamage(int dmg);
};