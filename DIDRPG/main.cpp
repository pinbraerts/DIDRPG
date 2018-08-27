#include "Creature.hpp"

#include "Test.hpp"

struct Poison : EffectHelper<Poison> {
	void ApplyDamage(float& damage) const {
		damage += 10;
	}
};
struct SmartPoison : EffectHelper<SmartPoison> {
	void ApplyDamage(float& damage) const {
		damage += 10;
	}
	void ReverseDamage(float& damage) const {
		damage -= 10;
	}
};

constexpr size_t N = 10, M = 10000;

void item_test(Creature& c, const Effect& cls) {
	size_t x = M;
	while (x--)
		c.addItem(cls);
	x = M;
	while (x--) {
		c.removeItem(cls);
		c.turn();
	}
}

int main() {
	Creature c;
	Poison p;
	SmartPoison sp;

	test << N << std::bind(item_test, c, p) << std::bind(item_test, c, sp);
	return 0;
}
