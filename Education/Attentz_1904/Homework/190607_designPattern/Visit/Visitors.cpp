#include "Visitors.h"
#include "Places.h"


void ElfVisitor::visit(class Inn* place) {
	std::cout << "An elf visited inn " + place->getName() + "." << std::endl;
	++mood;
}

void ElfVisitor::visit(class Market* place) {
	std::cout << "An elf visited market " + place->getName() + "." << std::endl;
	--mood;
}

void ElfVisitor::visit(class ElvenTemple* place) {
	std::cout << "An elf elven temple " + place->getName() + "." << std::endl;
	mood += 2;
}

void OrcVisitor::visit(class Inn* place) {
	std::cout << "An orc visited inn " + place->getName() + "." << std::endl;
	++mood;
}

void OrcVisitor::visit(class Market* place) {
	std::cout << "An orc visited market " + place->getName() + "." << std::endl;
	--mood;
}

void OrcVisitor::visit(class ElvenTemple* place) {
	std::cout << "Orcs cannot elven temple " + place->getName() + "." << std::endl;
}