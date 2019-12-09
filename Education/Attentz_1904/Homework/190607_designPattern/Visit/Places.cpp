#include "Places.h"
#include "Visitors.h"

void Places::add(Places* sub) {
	subPlaces.emplace_back(sub);
};

void Inn::accept(Visitor* _visitor) {
	_visitor->visit(this);
	if (!subPlaces.empty()) {
		for (auto elem : subPlaces) {
			elem->accept(_visitor);
		}
	}
}

void Market::accept(Visitor* _visitor) {
	_visitor->visit(this);
	if (!subPlaces.empty()) {
		for (auto elem : subPlaces) {
			elem->accept(_visitor);
		}
	}
}

void ElvenTemple::accept(Visitor* _visitor) {
	_visitor->visit(this);
	if (!subPlaces.empty()) {
		for (auto elem : subPlaces) {
			elem->accept(_visitor);
		}
	}
}

