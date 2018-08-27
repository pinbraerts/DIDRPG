#ifndef EFFECT_HPP
#define EFFECT_HPP

#include "includes.hpp"

struct Effect {
	template<size_t offset> using Changer = Callable<const Effect, offset, void, float&>;

private:
	Changer<0> ApplyDamage;
	Changer<sizeof(Effect::ApplyDamage)> ApplyProtection;
	Changer<sizeof(Effect::ApplyDamage) + sizeof(Effect::ApplyProtection)> ReverseDamage;
	Changer<sizeof(Effect::ApplyDamage) + sizeof(Effect::ApplyProtection) + sizeof(Effect::ReverseDamage)> ReverseProtection;

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

	Effect(decltype(ApplyDamage) a = nullptr, decltype(ApplyProtection) b = nullptr, decltype(ReverseDamage) c = nullptr, decltype(ReverseProtection) d = nullptr) :
		ApplyDamage(a), ApplyProtection(b),
		ReverseDamage(c), ReverseProtection(d) {}

	bool operator<(const Effect& other) const {
		return this < &other;
	}
};

template<class Origin> struct EffectHelper : Effect {
	void ApplyDamage(float&) const {}
	void ApplyProtection(float&) const {}
	void ReverseDamage(float&) const {}
	void ReverseProtection(float&) const {}

	static void _CApplyDamage(const Effect& self, float& f) {
		static_cast<const Origin&>(self).ApplyDamage(f);
	}
	static void _CApplyProtection(const Effect& self, float& f) {
		static_cast<const Origin&>(self).ApplyProtection(f);
	}
	static void _CReverseDamage(const Effect& self, float& f) {
		static_cast<const Origin&>(self).ReverseDamage(f);
	}
	static void _CReverseProtection(const Effect& self, float& f) {
		static_cast<const Origin&>(self).ReverseProtection(f);
	}

	EffectHelper() : Effect(
		&Origin::ApplyDamage != &EffectHelper::ApplyDamage ? &_CApplyDamage : nullptr,
		&Origin::ApplyProtection != &EffectHelper::ApplyProtection ? &_CApplyProtection : nullptr,
		&Origin::ReverseDamage != &EffectHelper::ReverseDamage ? &_CReverseDamage : nullptr,
		&Origin::ReverseProtection != &EffectHelper::ReverseProtection ? &_CReverseProtection : nullptr
	) {}
};

#endif // !EFFECT_HPP
