#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "windows.h"


class Maze
{
public:
	enum {
		UP = 1,
		RIGHT = 2,
		DOWN = 4,
		LEFT = 8
	};
	enum {
		ROBOT = 2,
		SHAPE_SIZE = 16,
		MAZE_SIZE = 19,
	};

	Maze();
	~Maze();

	void loadMaze(std::string path);
	void drawMaze() const;
	int getTile(int x, int y);

	class Mouse* mouse = nullptr;

protected:
	TCHAR getShape(int x, int y) const;

	int map[MAZE_SIZE][MAZE_SIZE]{ {0} };
	std::string path;

	std::vector<unsigned int> shape;
	std::ifstream ifs;
};

