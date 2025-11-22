#pragma once
#include "Player.h"
#include "Timer.h"

enum class LeftCenterRight{
	LEFT = 0, 
	CENTER = 1, 
	RIGHT = 2, 
};

enum class UpCenterDown
{
	UP = 0, 
	CENTER = 1, 
	DOWN = 2, 
};

class Enemy : public Player
{
private:
	Timer* timer;

public:
	Enemy(Vector2 pos, LeftCenterRight _xArea, UpCenterDown _yArea);
	void Draw(Vector2 offset)override;
	void Kill();
	void BeHurt();
	void IsDead();
	void WaitForNextActionLoop();
	void DoneActing();

	LeftCenterRight xArea;
	UpCenterDown yArea;
	int timeForNextMove;
	bool iCanMove;
	int health;
};