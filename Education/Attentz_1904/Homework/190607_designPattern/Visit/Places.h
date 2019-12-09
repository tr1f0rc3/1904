#pragma once
#include <string>
#include <vector>

class Places
{
public:
	Places() = default;
	Places(const std::string& _name) : name(_name) {};

	inline std::string getName() {
		return name;
	}
	virtual void accept(class Visitor* _visitor) = 0;
	void add(Places* sub);

protected:
	std::string name;
	std::vector<Places*> subPlaces;

};

class Inn : public Places {
public:
	Inn(const std::string& _name) : Places(_name) {};

	void accept(class Visitor* _visitor);
};

class Market : public Places {
public:
	Market(const std::string& _name) : Places(_name) {};

	void accept(class Visitor* _visitor);

protected:
};

class ElvenTemple : public Places {
public:
	ElvenTemple(const std::string& _name) : Places(_name) {};

	void accept(class Visitor* _visitor);
};

