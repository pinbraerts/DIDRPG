#include <iostream>
#include <chrono>
#include <functional>

#include "Player.hpp"

struct Poison : ItemClassHelper<Poison> {
	void ApplyDamage(float& damage) const {
		damage += 10;
	}
};
struct SmartPoison : ItemClassHelper<SmartPoison> {
	void ApplyDamage(float& damage) const {
		damage += 10;
	}
	void ReverseDamage(float& damage) const {
		damage += 10;
	}
};

struct Tester {
	size_t times = 1;

	Tester& operator<<(size_t n) {
		times = n;
		return *this;
	}

	Tester& operator<<(std::function<void()> callable) {
		size_t x = times;
		auto start = std::chrono::system_clock::now();
		while(x--) callable();
		auto end = std::chrono::system_clock::now();

		std::chrono::duration<double> elapsed_seconds = end - start;

		std::cout << "Elapsed time: " << elapsed_seconds.count() << 's' << std::endl;
		return *this;
	}
};

Tester test;
constexpr size_t N = 10, M = 10000;

void item_test(Creature& c, const ItemClass& cls) {
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
