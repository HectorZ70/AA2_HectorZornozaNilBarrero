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

bool Player::IsDead()
{
    if(GetHP() <= 0)
        return true;
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

bool Player::Attack(EDirection dir, std::vector<Enemy*>& enemies)
{
    int dx = 0, dy = 0;
    weaponDamage = 20;

    switch (dir)
    {
    case UP:    dy = -1; break;
    case DOWN:  dy = 1; break;
    case LEFT:  dx = -1; break;
    case RIGHT: dx = 1; break;
    }

    Vector2 p = GetPosition();

    for (int i = 1; i <= weaponRange; i++)
    {
        Vector2 checkPos = { p.X + dx * i, p.Y + dy * i };

        auto it = enemies.begin();
        while (it != enemies.end())
        {
            Enemy* e = *it;

            if (e->GetPosition().X == checkPos.X && e->GetPosition().Y == checkPos.Y)
            {
                e->TakeDamage(weaponDamage);

                if (e->IsDead())
                {
                    e->Stop(); 
                    delete e;  
                    it = enemies.erase(it); 
                }
                else
                {
                    ++it; 
                }

                return true; 
            }
            else
            {
                ++it; 
            }
        }
    }
    return false;
}

void Player::TakeDamage(int dmg)
{
    c_hp -= dmg;
}


Player::Player()
{
    _x = 5;
    _y = 5;
    c_hp = 100;
    c_potions = 4; 
    weaponRange = 1;
    _lastActionTime = std::chrono::steady_clock::now();
}

Player::~Player()
{
    //if (IsDead())
        
}