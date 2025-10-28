#include "DungeonMap.h"
#include "DungeonContent.h"

DungeonMap::DungeonMap(Vector2 size, Vector2 offset)
{
	_map = new NodeMap(size, offset);
	SetupMap(size);
}

DungeonMap::~DungeonMap()
{
	//delete _map;
}

void DungeonMap::Draw()
{
	_map->UnSafeDraw();
}

NodeMap* DungeonMap::GetNodeMap()
{
	return _map;
}

// Simula la creación de paredes y portales
void DungeonMap::SetupMap(Vector2 size)
{
	// Las dimensiones de la cuadrícula
	int maxX = size.X - 1;
	int maxY = size.Y - 1;

	for (int x = 0; x < size.X; x++)
	{
		for (int y = 0; y < size.Y; y++)
		{
			Vector2 pos(x, y);

			_map->SafePickNode(pos, [&](Node* node)
				{
					if (node == nullptr) return;

					// Lógica de Paredes (borde)
					if (x == 0 || x == maxX || y == 0 || y == maxY)
					{
						// Lógica de Portales: en las esquinas centrales del borde
						if ((x == 0 && y == maxY / 2) || (x == maxX && y == maxY / 2) ||
							(y == 0 && x == maxX / 2) || (y == maxY && x == maxX / 2))
						{
							node->SetContent(new DungeonContent(TileType::Portal, 'P'));
						}
						else // Pared en el resto del borde
						{
							node->SetContent(new DungeonContent(TileType::Wall, '#'));
						}
					}
					else // Espacio vacío
					{
						node->SetContent(new DungeonContent(TileType::Empty, '.'));
					}
				});
		}
	}
}