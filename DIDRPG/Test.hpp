#ifndef TEST_HPP
#define TEST_HPP

#include <functional>
#include <chrono>
#include <iostream>

struct Tester {
	size_t times = 1;

	Tester& operator<<(size_t n) {
		times = n;
		return *this;
	}

	Tester& operator<<(std::function<void()> callable) {
		size_t x = times;
		auto start = std::chrono::system_clock::now();
		while (x--) callable();
		auto end = std::chrono::system_clock::now();

		std::chrono::duration<double> elapsed_seconds = end - start;

		std::cout << "Elapsed time: " << elapsed_seconds.count() << 's' << std::endl;
		return *this;
	}
};

Tester test;

#endif // !TEST_HPP
