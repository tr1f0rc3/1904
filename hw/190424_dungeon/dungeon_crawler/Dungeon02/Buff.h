#pragma once

#ifndef BUFF_DEFINE
#define BUFF_DEFINE
enum class BuffType {
	none,
	inherited,
	gear, consumable,
	bless, magic, poison, curse,
	size,
};
enum class BuffEffect {
	none,
	damage,
	changeGauge,
	changeStats, changeStatsRatio,
	changeGearDurability,
	size,
};
using BuffEffectMask = std::array<bool, static_cast<int>(BuffEffect::size)>;
#endif

struct Buff;

template<>
void Table<Buff>::init() {
	Buff DeadlyPoison("b0001", "Deadly Poison", BuffType::poison, 5,
		Gauge<int>{0, 0}, Gauge<float>{-5.0, 0.0}, Stats<int>{0, 0}, Stats<float>{0.0, 0.0},
		"it is really really dangerous.");
		table.insert("b0001", DeadlyPoison);
}


#define GET_BUFF_TABLE(n) ( Table<Buff>::getInstance()->getTable((n)) )



struct Buff : public InfoBase {
	Buff(std::string _id, std::string _name, BuffType _type, int _duration,
		Gauge<int> _gauge, Gauge<float> _gaugeRatio,
		Stats<int> _stats, Stats<float> _statsRatio,
		std::string _toolTip = "");
	// get set
	BuffType getType() const { return type; }
	int getDuration() const { return duration; }
	BuffEffectMask getEffectMask() const { return effectMask; }
	// get spec
	Gauge<int> getGauge() const { return gauge; }
	Stats<int> getStats() const { return stats; }
	Stats<float> getStatsRatio() const { return statsRatio; }
private:
	// general info
	BuffType type = BuffType::none;
	int duration = 0;
	BuffEffectMask effectMask{ 0, };
	// maybe the buff have actual effect or not
	Gauge<int> gauge;
	Gauge<float> gaugeRatio;
	Stats<int> stats;
	Stats<float> statsRatio;
	// gearDurability is not implemented
};

Buff::Buff(std::string _id, std::string _name, BuffType _type, int _duration,
	Gauge<int> _gauge, Gauge<float> _gaugeRatio,
	Stats<int> _stats, Stats<float> _statsRatio,
	std::string _toolTip = "")
	: InfoBase(_id, _name, _toolTip), type(_type), duration(_duration),
	gauge(_gauge), gaugeRatio(_gaugeRatio), stats(_stats), statsRatio(_statsRatio) {

}


struct CreatedBuff : public CreatedObj {
public:
	CreatedBuff(int _id, Buff* _buffInfo, CreatedObj* _caster)
		: CreatedObj(_id), buff(_buffInfo), caster(_caster) {}
	Buff* getBuffInfo() const { return buff; }
private:
	Buff* buff = nullptr;
	CreatedObj* caster = nullptr;
};

struct CreatedBuffComposite : public Composite<CreatedBuff> { };

