#pragma once

class InfoBase {
public:
	InfoBase(std::string _id, std::string _name, std::string _toolTip = "")
		: id(_id), name(_name), toolTip(_toolTip) {}
	InfoBase(const InfoBase& _input) = delete;
	InfoBase(InfoBase&& _input) = delete;
	virtual ~InfoBase() {}
	
	std::string getId() { return id; }
	std::string getName() const { return name; }
	std::string getToolTip() const { return toolTip; }

private:
	std::string id = 0; // this id is different from id of createdObj
	std::string name;
	std::string toolTip;
};