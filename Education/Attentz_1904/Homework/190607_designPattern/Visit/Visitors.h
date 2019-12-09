#pragma once
#include <iostream>

class Visitor {
public:
	virtual void visit(class Inn* place) = 0;
	virtual void visit(class Market* place) = 0;
	virtual void visit(class ElvenTemple* place) = 0;


protected:
	int mood = 10;
};

class ElfVisitor : public Visitor {
public:
	void visit(class Inn* place);
	void visit(class Market* place);
	void visit(class ElvenTemple* place);


};

class OrcVisitor : public Visitor {
public:
	void visit(class Inn* place);
	void visit(class Market* place);
	void visit(class ElvenTemple* place);

};
