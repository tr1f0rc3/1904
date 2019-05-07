#pragma once

#define SAFE_DELETE(x)		{ if((x) != nullptr ) { delete (x); (x) = nullptr; } }
#define SAFE_DELETE_ARR(x)	{ if((x) != nullptr ) { delete[] (x); (x) = nullptr; } }


// �ִ� y�� ���� 20
// �α� ��ġ 24

enum class eObjectType
{
	None = 0,

	Wall,
	Item,

	Hero,
	Monster,
	DeadBody,
};

struct Point
{
	Point(int _x, int _y) : x{ _x }, y{ _y } {}

	int x = 0;
	int y = 0;

	bool operator==(const Point& rhs) const {
		return(x == rhs.x && y == rhs.y);
	}
};


/*

	enum class eKey
	{
		None = 0,

		Left,
		Up,
		Right,
		Down,
	};

	enum class eScene
	{
		None,

		Intro,
		Game,
	};
*/