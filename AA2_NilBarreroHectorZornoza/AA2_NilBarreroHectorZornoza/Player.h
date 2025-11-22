#pragma once
#include <mutex>
#include <chrono>

enum EDirection
{
	UP = 0, 
	DOWN = 1, 
	LEFT = 2, 
	RIGHT = 3, 
};

class Player
{
private:
	Player* player;
	int _x, _y = 0;
	std::mutex _mutexPlayer;
	std::chrono::steady_clock::time_point _lastActionTime;
	std::chrono::milliseconds _coolDown{ 300 };
	int c_hp;
	int c_coins;
	int c_potions;

public:
	Player();
	~Player();
	void Move(int key);
	void PrintPosition();
};

