#include <cassert>
#include "Maze.h"
#include "Mouse.h"


Maze::Maze()
{
	path = "maze01.txt";
	loadMaze(path);
	shape = { 32, (int)('1'), (int)('-'), (int)('2'), (int)('1'), (int)('1'), (int)('r'), (int)('k'), (int)('-'), (int)('j'), (int)('-'), (int)('4'), (int)('7'), (int)('3'), (int)('T'), (int)('+') };
	assert(shape.size() == 16);
}

Maze::~Maze()
{
}

void Maze::loadMaze(std::string path) {
	ifs.open(path);
	if (ifs.is_open()) {
		size_t row(0);
		while (!ifs.eof()) {
			assert(row < MAZE_SIZE);
			std::string tempString;
			getline(ifs, tempString);
			assert(tempString.length() <= MAZE_SIZE);
			size_t column(0);
			for (char elem : tempString) {
				assert(elem == '0' || elem == '1');
				map[row][column] = static_cast<int>(elem - '0');
				++column;
			}
			++row;
		}
	}
}

TCHAR Maze::getShape(int x, int y) const {
	unsigned int s(0);
	if (map[y][x]) {
		if (y > 0 && map[y - 1][x]) {
			s |= UP;
		}
		if (y < MAZE_SIZE - 2 && map[y + 1][x]) {
			s |= DOWN;
		}
		if (x > 0 && map[y][x - 1]) {
			s |= LEFT;
		}
		if (x < MAZE_SIZE - 2 && map[y][x + 1]) {
			s |= RIGHT;
		}
	}
	assert(s < SHAPE_SIZE);
	TCHAR newChar = (TCHAR)(shape[s]);
	return newChar;
}

void Maze::drawMaze() const {
	TCHAR tempTCHAR;
	assert(mouse != nullptr);
	int mouseX = mouse->getPos().first;
	int mouseY = mouse->getPos().second;
	system("cls");
	for (int i = 0; i < MAZE_SIZE; ++i) {
		for (int j = 0; j < MAZE_SIZE; ++j) {
			if (i == mouseY && j == mouseX) {
				tempTCHAR = '@';
			}
			else {
				tempTCHAR = getShape(j, i);
			}
			std::wcout << tempTCHAR;
		}
		std::wcout << std::endl;
	}
}

int Maze::getTile(int x, int y) {
	assert(x >= 0 && x < Maze::MAZE_SIZE && y >= 0 && y < Maze::MAZE_SIZE);
	return map[y][x];
}
