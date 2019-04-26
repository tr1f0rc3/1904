#pragma once
#include "Scene.h"

class Scene0 : public Scene {
public:

	Scene0();
	virtual ~Scene0() {}

	virtual void getInfo() const override;

	virtual bool init() override;
	virtual bool getInput(SceneMaster::keyInput _input) override;
	virtual bool update() override;
	virtual bool render() const override;


	int sc0Cnt = 0;
	std::string printText = "";
};
