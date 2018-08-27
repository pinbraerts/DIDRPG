#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <map>
#include <memory>

#include "includes.hpp"
#include "Callable.hpp"
#include "Effect.hpp"

struct CreatureBase {
	float health = 100;

public:
	float _damage = 0;
	float _protection = 0;

public:
	float damage() { return _damage; }
	float protection() { return _protection; }

	template<Field f> float& get();
	template<> float& get<Field::Damage>() { return _damage; }
	template<> float& get<Field::Protection>() { return _protection; }

	template<Field f> const float& get() const;
	template<> const float& get<Field::Damage>() const { return _damage; }
	template<> const float& get<Field::Protection>() const { return _protection; }

	template<Field f> bool ShouldRecount() const { return get<f>() == 0; }
	template<Field f> void SetRecount() { get<f>() = 0; }
};

template<class T> bool operator<(std::reference_wrapper<T> x, std::reference_wrapper<T> y) {
	return x.get() < y.get();
}

using Inventory = std::map<std::reference_wrapper<const Effect>, size_t>;

struct Creature: CreatureBase {
	Inventory inventory;

	template<Field f> inline
	void tryApply(const Effect& cls) {
		if (cls.IsChanging<f>())
			cls.Apply<f>(get<f>());
	}

	template<Field f> inline
	void recount() {
		for (auto item : inventory)
			tryApply<f>(item.first);
	}

	template<Field f> inline
	void tryReverse(const Effect& cls) {
		if (ShouldRecount<f>()) return;
		if (cls.CanReverse<f>())
			cls.Reverse<f>(get<f>());
		else SetRecount<f>();
	}

	void addItem(const Effect& item) {
		inventory[std::ref(item)] += 1;

		tryApply<Field::Damage>(item);
		tryApply<Field::Protection>(item);
	}
	void removeItem(const Effect& cls) {
		auto it = inventory.find(cls);
		if (it != inventory.end() && (--it->second) == 0)
			inventory.erase(it);

		tryReverse<Field::Damage>(cls);
		tryReverse<Field::Protection>(cls);
	}
	void turn() {
		if (ShouldRecount<Field::Damage>()) recount<Field::Damage>();
		// if (ShouldRecount<Field::Protection>()) recount<Field::Protection>();
	}
};

#endif // !PLAYER_HPP
