#pragma once

enum class BowType {
	Default,
	Long,
};

class Bow
{
public:
	virtual ~Bow() {}
	virtual void use() {
		std::cout << "use bow" << std::endl;
	}
};

class LongBow : public Bow
{
public:
	virtual ~LongBow() {}
	virtual void use() override {
		std::cout << "use long bow" << std::endl;
	}
};

class Archer
{
public:
	virtual ~Archer() {
		delete bow;
		bow = nullptr;
	}

	Bow* bow;
	virtual Bow* crateBow(BowType bowType) {
		switch (bowType) {
		case BowType::Default: {
			auto newBow = new Bow();
			bow = newBow;
			return newBow;
			break;
		}
		case BowType::Long: {
			auto newBow = new LongBow();
			bow = newBow;
			return newBow;
			break;
		}
		default:
			break;
		}
		return nullptr;
	}

	virtual void shoot() {
		bow->use();
	}

};