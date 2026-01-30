#include "Enemy.h"
#include "DungeonContent.h"
#include "DungeonMap.h"

void Enemy::MoveAI()
{
    auto now = std::chrono::steady_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - _lastActionTime);

    if (diff < _coolDown)
        return;

    int dir = rand() % 4;
    Vector2 newPos = _pos;

    switch (dir)
    {
    case 0: newPos.X++; break;
    case 1: newPos.X--; break;
    case 2: newPos.Y++; break;
    case 3: newPos.Y--; break;
    }

    NodeMap* nodeMap = _map->GetNodeMap();
    bool canMove = false;

    nodeMap->SafePickNode(newPos, [&](Node* node)
        {
            if (!node) return;

            DungeonContent* content = node->GetContent<DungeonContent>();
            if (!content)
                canMove = true;
            else if (content->GetType() == TileType::Wall ||
                content->GetType() == TileType::Portal ||
                content->GetType() == TileType::Chest)
                canMove = false;
            else
                canMove = true;
        });

    if (canMove)
    {
        std::lock_guard<std::mutex> lock(_mutexEnemy);
        _pos = newPos;
        _lastActionTime = now;
    }
}

void Enemy::RunEnemies()
{
	while (running)
	{
		MoveAI();
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

void Enemy::TakeDamage(int dmg)
{
	e_hp -= dmg;

	if (e_hp <= 0)
	{
		e_hp = 0;
		std::cout << "Enemy defeated!\n";
	}

	else
	{
		std::cout << "Enemy HP: " << e_hp << "\n";
	}
}

int Enemy::GetDamage() const
{
    return dmg;
}


bool Enemy::IsDead() const
{
	return e_hp <= 0;
}
