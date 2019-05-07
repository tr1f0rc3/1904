#pragma once

class Command;
class CommandComposite;

class Command {
public:
	// cstr
	Command(DungeonMaster* _editor, CreatedObj* _recv)
		: editor(_editor), receiver(_recv), randomSeed(time(0)) { 
	}
	virtual ~Command() {}
	// get set
	void add(Command* _nextPtr) {
		if (next != nullptr) { next->add(_nextPtr); }
		else { next = _nextPtr; }
	}
	CreatedObj* getReceiver() const { return receiver; }
	time_t getSeed() const { return randomSeed; }
	// execute
	virtual void execute() = 0;
	virtual void undo() = 0;
	virtual void handle() { if (next != nullptr) next->execute(); }
	virtual void reverseHandle() { if (prev != nullptr) prev->undo(); }
	// ptr
	DungeonMaster* editor = nullptr;
	Command* prev = nullptr;
	Command* next = nullptr;
protected:
	CreatedObj* receiver = nullptr;
	time_t randomSeed = 0;
};

struct CommandComposite : public Command, public Composite<Command> {
	void execute() override {
		for (auto& elem : composite) {
			elem->execute();
		}
	}
	void undo() override {
		for (auto it = composite.rbegin(); it != composite.rend(); ++it) {
			(*it)->undo();
		}
	}
};

