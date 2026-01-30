#pragma once
#include <mutex>
#include <chrono>
#include "Vector2_.h"
#include <vector>
#include "Enemy.h"

class Enemy;

enum EDirection
{
    UP = 0,
    DOWN = 1,
    LEFT = 2,
    RIGHT = 3,
    NONE 
};

class Player
{
private:
    Player* player;
    int _x, _y;
    int weaponDamage;
    int weaponRange;
    std::mutex _mutexPlayer;
    std::chrono::steady_clock::time_point _lastActionTime;
    std::chrono::milliseconds _coolDown{ 300 };
    int c_hp = 75;
    int c_coins = 0;
    int c_potions = 4;

public:
    Player();
    ~Player();
    void Move(int key);
    void PrintPosition();
    void DrinkPoption(int key);
    bool Attack(EDirection dir, std::vector<Enemy*>& enemies);
    void TakeDamage(int dmg);

    Vector2 GetPosition();
    int GetHP() { return c_hp; }
    int GetPotions() { return c_potions; }
    void SetHP(int hp) { _mutexPlayer.lock(); c_hp = hp; _mutexPlayer.unlock(); }
    void SetPotions(int potions) { _mutexPlayer.lock(); c_potions = potions; _mutexPlayer.unlock(); }
    void SetPosition(Vector2 pos);
    bool IsDead();
};