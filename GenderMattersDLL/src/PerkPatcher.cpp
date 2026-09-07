#include "PerkPatcher.h"
#include "Settings.h"

#include <RE/Skyrim.h>
#include <SKSE/Logger.h>

namespace
{
	using EP = RE::BGSEntryPoint::ENTRY_POINTS;

	RE::TESGlobal* FindGlobal(const char* a_editorID)
	{
		return RE::TESForm::LookupByEditorID<RE::TESGlobal>(a_editorID);
	}

	void SetGlobal(const char* a_editorID, bool a_value)
	{
		if (auto* g = FindGlobal(a_editorID)) {
			g->value = a_value ? 1.0f : 0.0f;
		} else {
			SKSE::log::warn("PerkPatcher: global '{}' not found - is gender matters.esp loaded?", a_editorID);
		}
	}

	// The perk ships exactly two numeric entries per entry-point type: the
	// normal one, then the hardcore one, always in that file order (verified
	// by decoding the shipped esp directly). We walk the array once and patch
	// each occurrence in order.
	void PatchMagnitudes(RE::BGSPerk* a_perk)
	{
		const auto& mag = Settings::Get().magnitudes;

		int attackDamageSeen = 0;
		int incomingDamageSeen = 0;
		int staggerSeen = 0;
		int blockSeen = 0;
		int intimidationSeen = 0;
		int patched = 0;

		for (auto* entry : a_perk->perkEntries) {
			auto* epEntry = skyrim_cast<RE::BGSEntryPointPerkEntry*>(entry);
			if (!epEntry || !epEntry->functionData) {
				continue;
			}

			auto* oneVal = skyrim_cast<RE::BGSEntryPointFunctionDataOneValue*>(epEntry->functionData);
			if (!oneVal) {
				// e.g. the ranged quickdraw entry, which is a boolean graph
				// variable, not a numeric magnitude - nothing to patch there.
				continue;
			}

			switch (epEntry->entryData.entryPoint.get()) {
			case EP::kModAttackDamage:
				oneVal->data = (attackDamageSeen++ == 0) ? mag.meleeDamageDealtMult : mag.meleeDamageDealtMultHardcore;
				++patched;
				break;
			case EP::kModIncomingDamage:
				oneVal->data = (incomingDamageSeen++ == 0) ? mag.incomingDamageMult : mag.incomingDamageMultHardcore;
				++patched;
				break;
			case EP::kModIncomingStagger:
				oneVal->data = (staggerSeen++ == 0) ? mag.incomingStaggerMult : mag.incomingStaggerMultHardcore;
				++patched;
				break;
			case EP::kModPercentBlocked:
				oneVal->data = (blockSeen++ == 0) ? mag.blockEffectivenessMult : mag.blockEffectivenessMultHardcore;
				++patched;
				break;
			case EP::kModPlayerIntimidation:
				oneVal->data = (intimidationSeen++ == 0) ? mag.intimidationMultHighSkill : mag.intimidationMultLowSkill;
				++patched;
				break;
			default:
				break;
			}
		}

		SKSE::log::info("PerkPatcher: rewrote {} perk entry-point magnitudes", patched);
	}
}

void PerkPatcher::Apply()
{
	const auto& t = Settings::Get().toggles;

	SetGlobal("GM_Enabled", t.enabled);
	SetGlobal("GM_AffectsSkills", t.affectsSkills);
	SetGlobal("GM_LessCarry", t.lessCarry);
	SetGlobal("GM_BlockLessDmg", t.blockLessDmg);
	SetGlobal("GM_GetStaggeredMore", t.getStaggeredMore);
	SetGlobal("GM_SneakBetter", t.sneakBetter);
	SetGlobal("GM_InflictLessMelee", t.inflictLessMelee);
	SetGlobal("GM_TakeMoreDmg", t.takeMoreDmg);
	SetGlobal("GM_RangedNerf", t.rangedNerf);
	SetGlobal("GM_Smithing", t.smithingMalus);
	SetGlobal("GM_Hardcore", t.hardcore);

	auto* perk = RE::TESForm::LookupByEditorID<RE::BGSPerk>("GM_Perk_Combat");
	if (!perk) {
		SKSE::log::error("PerkPatcher: GM_Perk_Combat not found - is gender matters.esp loaded and enabled?");
		return;
	}

	PatchMagnitudes(perk);
}
