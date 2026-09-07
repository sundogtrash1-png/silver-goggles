#include "GenderDistributor.h"
#include "Settings.h"

#include <RE/Skyrim.h>
#include <SKSE/Logger.h>
#include <SKSE/SKSE.h>

namespace
{
	RE::BGSPerk* GetPerk()
	{
		static auto* perk = RE::TESForm::LookupByEditorID<RE::BGSPerk>("GM_Perk_Combat");
		return perk;
	}

	bool IsFemale(RE::Actor* a_actor)
	{
		const auto* base = a_actor ? a_actor->GetActorBase() : nullptr;
		return base && base->IsFemale();
	}

	// Applies (a_add=true) or reverses (a_add=false) the flat AV bonuses
	// that ride alongside the perk. Direction is just the sign of the
	// configured magnitude, so calling this twice with opposite a_add
	// cancels out exactly.
	void ApplyFlatBonuses(RE::Actor* a_actor, bool a_add)
	{
		auto* avOwner = a_actor->AsActorValueOwner();
		if (!avOwner) {
			return;
		}

		const auto& t = Settings::Get().toggles;
		const auto& m = Settings::Get().magnitudes;
		const float sign = a_add ? 1.0f : -1.0f;

		if (t.lessCarry) {
			avOwner->ModActorValue(RE::ActorValue::kCarryWeight, sign * m.carryWeightFlat);
		}
		if (t.magicResistMalus) {
			const float v = t.hardcore ? m.magicResistFlatHardcore : m.magicResistFlat;
			avOwner->ModActorValue(RE::ActorValue::kResistMagic, sign * v);
		}
		if (t.sneakBetter) {
			avOwner->ModActorValue(RE::ActorValue::kSneak, sign * m.sneakBonusFlat);
		}
		if (t.affectsSkills) {
			avOwner->ModActorValue(RE::ActorValue::kSpeech, sign * m.speechBonusFlat);
			avOwner->ModActorValue(RE::ActorValue::kAlchemy, sign * m.alchemyBonusFlat);
		}
		if (t.smithingMalus) {
			avOwner->ModActorValue(RE::ActorValue::kSmithing, sign * m.smithingMalusFlat);
		}
	}

	bool ShouldHaveEffects(RE::Actor* a_actor)
	{
		const auto& t = Settings::Get().toggles;
		if (!t.enabled) {
			return false;
		}
		const bool isPlayer = a_actor->IsPlayerRef();
		if (isPlayer && !t.applyToPlayer) {
			return false;
		}
		if (!isPlayer && !t.applyToNPCs) {
			return false;
		}
		return IsFemale(a_actor);
	}

	class LoadedEventSink : public RE::BSTEventSink<RE::TESObjectLoadedEvent>
	{
	public:
		static LoadedEventSink& GetSingleton()
		{
			static LoadedEventSink singleton;
			return singleton;
		}

		RE::BSEventNotifyControl ProcessEvent(const RE::TESObjectLoadedEvent* a_event,
			RE::BSTEventSource<RE::TESObjectLoadedEvent>*) override
		{
			if (!a_event || !a_event->loaded) {
				return RE::BSEventNotifyControl::kContinue;
			}
			auto* form = RE::TESForm::LookupByID(a_event->formID);
			if (auto* actor = form ? form->As<RE::Actor>() : nullptr) {
				GenderDistributor::Sync(actor);
			}
			return RE::BSEventNotifyControl::kContinue;
		}
	};
}

void GenderDistributor::Sync(RE::Actor* a_actor)
{
	if (!a_actor) {
		return;
	}
	auto* perk = GetPerk();
	if (!perk) {
		return;
	}

	const bool shouldHave = ShouldHaveEffects(a_actor);
	const bool currentlyHas = a_actor->HasPerk(perk);

	if (shouldHave && !currentlyHas) {
		a_actor->AddPerk(perk);
		ApplyFlatBonuses(a_actor, true);
	} else if (!shouldHave && currentlyHas) {
		a_actor->RemovePerk(perk);
		ApplyFlatBonuses(a_actor, false);
	}
	// If shouldHave == currentlyHas, we're already in sync - do nothing,
	// since ApplyFlatBonuses is not idempotent (calling it twice in the
	// same direction would double-apply the flat bonuses).
}

void GenderDistributor::Install()
{
	if (auto* source = RE::ScriptEventSourceHolder::GetSingleton()) {
		source->AddEventSink(&LoadedEventSink::GetSingleton());
		SKSE::log::info("GenderDistributor: TESObjectLoadedEvent sink installed");
	}
}
