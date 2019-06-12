// Decorator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "MagicOrb.h"
using namespace std;

// 1. "lowest common denominator"
class Widget
{
public:
	virtual void draw() = 0;
};

class TextField : public Widget
{
	// 3. "Core" class & "is a"
	int width, height;
public:
	TextField(int w, int h)
	{
		width = w;
		height = h;
	}

	/*virtual*/
	void draw()
	{
		cout << "TextField: " << width << ", " << height << '\n';
	}
};

// 2. 2nd level base class
class Decorator : public Widget  // 4. "is a" relationship
{
	Widget *wid; // 4. "has a" relationship
public:
	Decorator(Widget *w)
	{
		wid = w;
	}

	/*virtual*/
	void draw()
	{
		wid->draw(); // 5. Delegation
	}
};

class BorderDecorator : public Decorator
{
public:
	// 6. Optional embellishment
	BorderDecorator(Widget *w) : Decorator(w) {}

	/*virtual*/
	void draw()
	{
		// 7. Delegate to base class and add extra stuff
		Decorator::draw();
		cout << "   BorderDecorator" << '\n';
	}
};

class ScrollDecorator : public Decorator
{
public:
	// 6. Optional embellishment
	ScrollDecorator(Widget *w) : Decorator(w) {}

	/*virtual*/
	void draw()
	{
		// 7. Delegate to base class and add extra stuff
		Decorator::draw();
		cout << "   ScrollDecorator" << '\n';
	}
};

int main()
{

	MagicOrb* magicOrb = new FireEnhancedOrb(new FireEnhancedOrb(new MagicOrb(10, 15)));
	magicOrb->activate();
	// 8. Client has the responsibility to compose desired configurations
	Widget *aWidget = new BorderDecorator(new BorderDecorator(new ScrollDecorator
	(new TextField(80, 24))));
	aWidget->draw();
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
