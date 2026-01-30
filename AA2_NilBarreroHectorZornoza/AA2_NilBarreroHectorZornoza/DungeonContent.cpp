#include "DungeonContent.h"
#include "ConsoleControl_.h" 

DungeonContent::DungeonContent(TileType type, char symbol)
	: _type(type), _symbol(symbol)
{
}

void DungeonContent::Draw(Vector2 offset)
{
	CC::SetPosition(offset.X, offset.Y);
	std::cout << _symbol;
}

TileType DungeonContent::GetType() const
{
	return _type;
}

char DungeonContent::GetSymbol() const
{
	return _symbol;
}