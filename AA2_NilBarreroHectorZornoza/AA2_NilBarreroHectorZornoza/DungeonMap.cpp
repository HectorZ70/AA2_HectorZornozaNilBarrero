#include "DungeonMap.h"
#include "DungeonContent.h"

DungeonMap::DungeonMap(Vector2 overworldPos, Vector2 size, Vector2 offset)
{
	_map = new NodeMap(size, offset);
	SetupMap(overworldPos, size);
}

DungeonMap::~DungeonMap()
{
	delete _map;
}

void DungeonMap::Draw()
{
	_map->UnSafeDraw();
}

NodeMap* DungeonMap::GetNodeMap()
{
	return _map;
}

void DungeonMap::SetupMap(Vector2 overworldPos, Vector2 size)
{
	int maxX = size.X - 1;
	int maxY = size.Y - 1;

	int maxOverworldX = 2; 
	int maxOverworldY = 2; 

	for (int x = 0; x < size.X; x++)
	{
		for (int y = 0; y < size.Y; y++)
		{
			Vector2 pos(x, y);

			_map->SafePickNode(pos, [&](Node* node)
			{
					if (node == nullptr) return;

					if (x == 0 || x == maxX || y == 0 || y == maxY)
					{
						bool isPortalSpot = (x == 0 && y == maxY / 2) || // Izquierda
							(x == maxX && y == maxY / 2) || // Derecha
							(y == 0 && x == maxX / 2) || // Arriba
							(y == maxY && x == maxX / 2); // Abajo

						if (isPortalSpot)
						{
							bool canGoLeft = (x == 0 && overworldPos.X > 0);
							bool canGoRight = (x == maxX && overworldPos.X < maxOverworldX);
							bool canGoUp = (y == 0 && overworldPos.Y > 0);
							bool canGoDown = (y == maxY && overworldPos.Y < maxOverworldY);

							if (canGoLeft || canGoRight || canGoUp || canGoDown)
							{
								node->SetContent(new DungeonContent(TileType::Portal, 'P'));
							}
							else
							{
								node->SetContent(new DungeonContent(TileType::Wall, '#'));
							}
						}
						else
						{
							node->SetContent(new DungeonContent(TileType::Wall, '#'));
						}
					}
					else
					{
						node->SetContent(new DungeonContent(TileType::Empty, ' '));
					}
			});
		}
	}
}