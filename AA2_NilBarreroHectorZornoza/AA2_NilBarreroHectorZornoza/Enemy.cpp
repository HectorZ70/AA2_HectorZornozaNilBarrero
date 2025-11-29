#include "Enemy.h"
#include "DungeonContent.h"

void Enemy::MoveAI()
{
	int dir = rand() % 4;
	Vector2 newPos = _pos;

	switch (dir)
	{
	case 0:
		newPos.X++;
		break;
	case 1:
		newPos.X--;
		break;
	case 2:
		newPos.Y++;
		break;
	case 3:
		newPos.Y--;
		break;
	default:
		break;
	}

	NodeMap* nodeMap = _map->GetNodeMap();

	bool canMove = false;

	nodeMap->SafePickNode(newPos, [&](Node* node)
		{
			if (node == nullptr) return;

			DungeonContent* content = node->GetContent<DungeonContent>();

			if (content == nullptr)
				canMove = true;
			else if (content->GetType() == TileType::Wall)
				canMove = false;
			else if (content->GetType() == TileType::Portal)
				canMove = false;
			else
				canMove = true;
		});

	if (canMove)
		_pos = newPos;
}
