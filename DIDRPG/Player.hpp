#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <bitset>
#include <vector>
#include <list>
#include <forward_list>
#include <memory>
#include <functional>
#include <map>

enum class Field {
	Damage, Protection
};

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

template<class Handler, size_t offset, class Ret, class... Args> class MethodCallable {
public:
	using PtrType = Ret(*)(Handler&, Args...);

	PtrType ptr;

	inline Handler& self() {
		return *reinterpret_cast<Handler*>(reinterpret_cast<unsigned char*>(this) - offset);
	}
	inline const Handler& self() const {
		return *reinterpret_cast<const Handler*>(reinterpret_cast<const unsigned char*>(this) - offset);
	}

public:
	MethodCallable(nullptr_t np): ptr(nullptr) {}
	MethodCallable(PtrType pointer) : ptr(pointer) {}

	inline Ret operator()(Args&&... args) const {
		if(ptr != nullptr)
			return (*ptr)(self(), std::forward<Args>(args)...);
		else throw std::bad_function_call{};
	}
	inline Ret operator()(Args&&... args) {
		if (ptr != nullptr)
			return (*ptr)(self(), std::forward<Args>(args)...);
		else throw std::bad_function_call{};
	}
	inline bool operator==(nullptr_t np) const {
		return ptr == np;
	}
	inline bool operator!=(nullptr_t np) const {
		return ptr != np;
	}
};

struct ItemClass {
	template<size_t offset> using Changer = MethodCallable<const ItemClass, offset, void, float&>;

private:
	Changer<0> ApplyDamage;
	Changer<sizeof(ItemClass::ApplyDamage)> ApplyProtection;
	Changer<sizeof(ItemClass::ApplyDamage) + sizeof(ItemClass::ApplyProtection)> ReverseDamage;
	Changer<sizeof(ItemClass::ApplyDamage) + sizeof(ItemClass::ApplyProtection) + sizeof(ItemClass::ReverseDamage)> ReverseProtection;

public:
	template<Field f> bool IsChanging() const;
	template<> bool IsChanging<Field::Damage>() const {
		return ApplyDamage != nullptr;
	}
	template<> bool IsChanging<Field::Protection>() const {
		return ApplyProtection != nullptr;
	}

	template<Field f> void Apply(float& field) const;
	template<> void Apply<Field::Damage>(float& field) const {
		return ApplyDamage(field);
	}
	template<> void Apply<Field::Protection>(float& field) const {
		return ApplyProtection(field);
	}

	template<Field f> bool CanReverse() const;
	template<> bool CanReverse<Field::Damage>() const {
		return ReverseDamage != nullptr;
	}
	template<> bool CanReverse<Field::Protection>() const {
		return ReverseProtection != nullptr;
	}

	template<Field f> void Reverse(float& field) const;
	template<> void Reverse<Field::Damage>(float& field) const {
		return ReverseDamage(field);
	}
	template<> void Reverse<Field::Protection>(float& field) const {
		return ReverseProtection(field);
	}

	ItemClass(decltype(ApplyDamage) a = nullptr, decltype(ApplyProtection) b = nullptr, decltype(ReverseDamage) c = nullptr, decltype(ReverseProtection) d = nullptr):
		ApplyDamage(a), ApplyProtection(b),
		ReverseDamage(c), ReverseProtection(d) {}

	bool operator<(const ItemClass& other) const {
		return this < &other;
	}
};

template<class Origin> struct ItemClassHelper: ItemClass {
	void ApplyDamage(float&) const {}
	void ApplyProtection(float&) const {}
	void ReverseDamage(float&) const {}
	void ReverseProtection(float&) const {}

	static void _CApplyDamage(const ItemClass& self, float& f) {
		static_cast<const Origin&>(self).ApplyDamage(f);
	}
	static void _CApplyProtection(const ItemClass& self, float& f) {
		static_cast<const Origin&>(self).ApplyProtection(f);
	}
	static void _CReverseDamage(const ItemClass& self, float& f) {
		static_cast<const Origin&>(self).ReverseDamage(f);
	}
	static void _CReverseProtection(const ItemClass& self, float& f) {
		static_cast<const Origin&>(self).ReverseProtection(f);
	}

	ItemClassHelper() : ItemClass(
		&Origin::ApplyDamage != &ItemClassHelper::ApplyDamage ? &_CApplyDamage : nullptr,
		&Origin::ApplyProtection != &ItemClassHelper::ApplyProtection ? &_CApplyProtection : nullptr,
		&Origin::ReverseDamage != &ItemClassHelper::ReverseDamage ? &_CReverseDamage : nullptr,
		&Origin::ReverseProtection != &ItemClassHelper::ReverseProtection ? &_CReverseProtection : nullptr
	) {}
};

template<class T> bool operator<(std::reference_wrapper<T> x, std::reference_wrapper<T> y) {
	return x.get() < y.get();
}

using Inventory = std::map <std::reference_wrapper<const ItemClass>, size_t>;

struct Creature: CreatureBase {
	Inventory inventory;

	template<Field f>
	void tryApply(const ItemClass& cls) {
		if (cls.IsChanging<f>())
			cls.Apply<f>(get<f>());
	}

	template<Field f>
	void recount() {
		for (auto item : inventory)
			tryApply<f>(item.first);
	}

	template<Field f>
	void tryReverse(const ItemClass& cls) {
		if (ShouldRecount<f>()) return;
		if (cls.CanReverse<f>())
			cls.Reverse<f>(get<f>());
		else SetRecount<f>();
	}

	void addItem(const ItemClass& item) {
		inventory[std::ref(item)] += 1;

		tryApply<Field::Damage>(item);
		tryApply<Field::Protection>(item);
	}
	void removeItem(const ItemClass& cls) {
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
