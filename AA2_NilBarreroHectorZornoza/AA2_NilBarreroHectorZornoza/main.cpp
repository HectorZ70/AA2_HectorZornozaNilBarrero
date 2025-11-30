#include "OverworldMap.h"
#include "ConsoleControl_.h"
#include <iostream>

int main()
{
	srand(time(NULL));
	std::cout << "Inicializando el sistema de Mapas...\n";

	// Tamaño de cada mapa: 10x10 nodos
	Vector2 cellSize(10, 10);

	// El mapa grande es 3x3
	Vector2 mapSize(3, 3);

	OverworldMap gameMap(mapSize, cellSize);

	InputSystem input;

	Player player;

	std::cout << "Mapa inicializado (3x3). Presione 'Q' para salir.\n";

	// Bucle principal del juego
	gameMap.Run(input, player);

	std::cout << "Saliendo del juego.\n";

	return 0;
}