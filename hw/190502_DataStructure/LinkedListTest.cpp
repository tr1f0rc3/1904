#include "pch.h"
#include "LinkedList.h"
#include "Stack.h"


int main()
{

	using namespace std;
	{
		Stack<int> St;
		St.Pop();
		St.Push(5);
		St.Push(6);
		St.Push(7);
		St.Push(8);
		St.Push(9);
		for (int i = 0; i != 6; ++i)
		{
			std::cout << St.Pop() << ' ';
		}
	}


	{
		LinkedList<int> LL;
		LL.AddNode(5);
		LL.AddNode(6);
		LL.AddNode(7);
		LL.AddNode(8);
		LL.AddNode(9);
		LL.PrintAll();

		LL.DeleteNode(7);
		LL.PrintAll();

		LL.DeleteNode(5);
		LL.PrintAll();

		LL.DeleteNode(9);
		LL.PrintAll();
	}


}