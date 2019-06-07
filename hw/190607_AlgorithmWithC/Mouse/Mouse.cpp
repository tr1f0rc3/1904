
#include <windows.h>
#include <cassert>
#include "Mouse.h"
#include "Maze.h"

Mouse::Mouse(Maze* _maze, int _x, int _y, unsigned int _dir) : maze(_maze), x(_x), y(_y), dir(_dir) {

}


Mouse::~Mouse()
{
}

std::pair<int, int> Mouse::getPos() {
	return std::make_pair(x, y);
}

void Mouse::record()
{
	path.emplace_back(std::make_pair(x, y));
}

void Mouse::record(int _x, int _y)
{
	path.emplace_back(std::make_pair(_x, _y));
}

void Mouse::forward()
{
	x += (dir == Maze::LEFT) ? -1 : (dir == Maze::RIGHT) ? 1 : 0;
	y += (dir == Maze::UP) ? -1 : (dir == Maze::DOWN) ? 1 : 0;
	record();
}

void Mouse::right() {
	dir <<= 1;
	if (dir > Maze::LEFT) {
		dir = Maze::UP;
	}
}

void Mouse::left() {
	dir >>= 1;
	if (dir == 0) {
		dir = Maze::LEFT;
	}
}

int Mouse::stillInMaze() const {
	if (x > 0 && x < Maze::MAZE_SIZE - 1 && y > 0 && y < Maze::MAZE_SIZE - 1) {
		return 1;
	}
	return 0;
}

int Mouse::wallAhead() const {
	int _x = (dir == Maze::LEFT) ? (x-1) : (dir == Maze::RIGHT) ? (x+1) : x;
	int _y = (dir == Maze::UP) ? (y-1) : (dir == Maze::DOWN) ? (y+1) : y;
	assert(maze != nullptr);
	return maze->getTile(_x, _y);
}

void Mouse::rightHand() {
	record();
	forward();
	while (stillInMaze()) {
		right();
		while (wallAhead()) {
			left();
		}
		forward();
	}
	record(-1, -1);
}

void Mouse::delPath(int i, int j) {
	while (path[j].first >= 0) {
		path[i++] = path[j++];
	}
	path[i] = std::make_pair(-1, -1);
}

void Mouse::shortestPath() {
	int i(0), j;
	std::pair<int, int> pos1, pos2;
	while (path[i].first >= 0) {
		assert(i < path.size());
		pos1 = path[i];
		j = i + 1;
		while (path[j].first >= 0) {
			assert(j < path.size());
			pos2 = path[j];
			if (pos1 == pos2) {
				delPath(i, j);
			}
			++j;
		}
		++i;
	}
}

void Mouse::drawPath() {
	int i(0);
	while (path[i].first >= 0) {
		x = path[i].first;
		y = path[i++].second;
		assert(maze != nullptr);
		maze->drawMaze();
		Sleep(100);
		assert(i < path.size());
	}
}