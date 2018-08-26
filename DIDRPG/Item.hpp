#ifndef ITEM_HPP
#define ITEM_HPP

#include "includes.hpp"

struct ItemClass {
	template<size_t offset> using Changer = Callable<const ItemClass, offset, void, float&>;

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

	ItemClass(decltype(ApplyDamage) a = nullptr, decltype(ApplyProtection) b = nullptr, decltype(ReverseDamage) c = nullptr, decltype(ReverseProtection) d = nullptr) :
		ApplyDamage(a), ApplyProtection(b),
		ReverseDamage(c), ReverseProtection(d) {}

	bool operator<(const ItemClass& other) const {
		return this < &other;
	}
};

template<class Origin> struct ItemClassHelper : ItemClass {
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

#endif // !ITEM_HPP
