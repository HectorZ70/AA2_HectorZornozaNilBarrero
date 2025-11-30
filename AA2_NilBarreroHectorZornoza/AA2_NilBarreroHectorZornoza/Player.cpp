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

Vector2 Player::GetPosition()
{
    _mutexPlayer.lock();
    Vector2 pos(_x, _y);
    _mutexPlayer.unlock();
    return pos;
}

void Player::SetPosition(Vector2 pos)
{
    _mutexPlayer.lock();
    _x = pos.X;
    _y = pos.Y;
    _mutexPlayer.unlock();
}

void Player::DrinkPoption(int key)
{
    if (key == K_Q && c_hp == 100 && c_potions > 0) {
        c_potions--;
    }

    else if (key == K_Q && c_potions > 0) {
        c_potions--;
        c_hp += 25;
    }
}

Player::Player()
{
    
}

Player::~Player()
{

}