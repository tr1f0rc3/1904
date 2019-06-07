// Mouse.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include <io.h>
#include <fcntl.h>
#include "Maze.h"
#include "Mouse.h"

int main()
{
	_setmode(_fileno(stdout), _O_U16TEXT);

	Maze maze;
	Mouse mouse(&maze, Maze::MAZE_SIZE - 1, Maze::MAZE_SIZE - 2, Maze::LEFT);
	maze.mouse = &mouse;
	maze.drawMaze();
	mouse.rightHand();
	mouse.drawPath();
	mouse.shortestPath();
	mouse.drawPath();
    std::cout << "Hello World!\n"; 
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
