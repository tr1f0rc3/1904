#pragma once
#include <vector>

class Mouse
{
public:
	Mouse(class Maze* _maze, int _x, int _y, unsigned int _dir);
	~Mouse();
	
	std::pair<int, int> getPos();
	void record();
	void record(int _x, int _y);
	void forward();
	void right();
	void left();
	int stillInMaze() const;
	int wallAhead() const;
	void rightHand();
	void delPath(int i, int j);
	void shortestPath();
	void drawPath();

	class Maze* maze = nullptr;

protected:
	std::vector<std::pair<int, int>> path;
	int x;
	int y;
	unsigned int dir;
};

