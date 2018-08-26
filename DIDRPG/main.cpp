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

int main() {
	Creature c;
	size_t N = 1000, M = 10;
	c._protection = 1; // not to recount

	test << N << [&] {
		Poison p;

		size_t x = M;
		while (x--)
			c.addItem(p);
		x = M;
		while (x--) {
			c.removeItem(p);
			c.turn();
		}
	} << [&] {
		SmartPoison p;

		size_t x = M;
		while (x--)
			c.addItem(p);
		x = M;
		while (x--) {
			c.removeItem(p);
			c.turn();
		}
	};
	return 0;
}
