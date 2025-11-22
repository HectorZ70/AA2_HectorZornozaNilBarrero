#include "Player.h"
#include "InputConsts.h"
#include "Vector2_.h"
#include "ConsoleControl_.h"
#include <iostream>

void Player::Move(int key)
{
	auto now = std::chrono::steady_clock::now();
	auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - _lastActionTime);

	if (diff < _coolDown)
		return;

	_mutexPlayer.lock();

	switch (key)
	{
	case K_W: 
		_y -= 1;
		break;
	case K_S:
		_y += 1;
		break;
	case K_A:
		_x -= 1;
		break;
	case K_D:
		_x += 1;
		break;
	default:
		break;
	}

	_lastActionTime = now;

	_mutexPlayer.unlock();
}

void Player::PrintPosition()
{
	_mutexPlayer.lock();
	std::cout << "Position : [" << _x << "] [" << _y << "]\n";
	_mutexPlayer.unlock();
}

Player::Player()
{

}

Player::~Player()
{

}
