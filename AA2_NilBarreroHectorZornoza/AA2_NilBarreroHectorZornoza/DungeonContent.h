#pragma once
#include "INodeContent.h"
#include <iostream>

// Enumeración para el tipo de contenido que tendrá el nodo
enum class TileType
{
	Empty,
	Wall,
	Chest,
	Portal
};

class DungeonContent : public INodeContent
{
public:
	DungeonContent(TileType type, char symbol);

	void Draw(Vector2 offset) override;

	TileType GetType() const;
	char GetSymbol() const;

private:
	TileType _type;
	char _symbol;
};