#pragma once

class TargetCmd;
class SkillCmd;

// alone-action

// interaction between characters
class TargetCmd : public Command {
public:
	TargetCmd(DungeonMaster* _editor, CreatedObj* _recv, Character* _target)
		: Command(_editor, _recv), target(_target) {}
	Character* getTarget() const { return target; }
protected:
	Character* target = nullptr;
};

class SkillCmd : public TargetCmd {
public:
	SkillCmd(DungeonMaster* _editor, CreatedObj* _recv,
		CreatedSkill* _skill, Character* _target = nullptr)
		: TargetCmd(_editor, _recv, _target), skill(_skill) {}
	void execute() override {
		srand(randomSeed);
		receiver->castSkill(skill, target);
	}
	void undo() override { }
protected:
	CreatedSkill* skill = nullptr;
};



