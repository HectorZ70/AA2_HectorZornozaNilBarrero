#pragma once
#include "INodeContent.h"
#include <iostream>

// Enumeración para el tipo de contenido que tendrá el nodo
enum class TileType
{
	Empty,
	Wall,
	Chest,
	Portal,
    Coins
};

class DungeonContent : public INodeContent
{
public:
    DungeonContent(TileType type, char symbol);

    void Draw(Vector2 offset) override;

    TileType GetType() const;
    char GetSymbol() const;

    // Nuevo: para cofres
    bool IsOpened() const { return _isOpened; }
    void SetOpened(bool val) { _isOpened = val; }

private:
    TileType _type;
    char _symbol;
    bool _isOpened = false; // Por defecto cerrado
};