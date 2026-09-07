#include "Settings.h"

#include <SKSE/Logger.h>

#include <fstream>
#include <string>
#include <unordered_map>

namespace
{
	Settings::Data g_data;

	std::string Trim(const std::string& s)
	{
		const auto a = s.find_first_not_of(" \t\r\n");
		if (a == std::string::npos) {
			return {};
		}
		const auto b = s.find_last_not_of(" \t\r\n");
		return s.substr(a, b - a + 1);
	}

	// Deliberately not using a library here: this is a handful of flat
	// key=value pairs, no sections/arrays needed, and it keeps the plugin's
	// dependency list at zero.
	std::unordered_map<std::string, std::string> ParseINI(const std::filesystem::path& a_path)
	{
		std::unordered_map<std::string, std::string> out;

		std::ifstream file(a_path);
		if (!file.is_open()) {
			SKSE::log::warn("Settings: no ini found at {}, using built-in defaults", a_path.string());
			return out;
		}

		std::string line;
		while (std::getline(file, line)) {
			line = Trim(line);
			if (line.empty() || line[0] == ';' || line[0] == '#' || line[0] == '[') {
				continue;
			}
			const auto eq = line.find('=');
			if (eq == std::string::npos) {
				continue;
			}
			std::string key = Trim(line.substr(0, eq));
			std::string val = Trim(line.substr(eq + 1));
			const auto semi = val.find(';');
			if (semi != std::string::npos) {
				val = Trim(val.substr(0, semi));
			}
			out[key] = val;
		}
		return out;
	}

	bool GetBool(const std::unordered_map<std::string, std::string>& a_ini, const char* a_key, bool a_default)
	{
		const auto it = a_ini.find(a_key);
		if (it == a_ini.end() || it->second.empty()) {
			return a_default;
		}
		return it->second == "1" || it->second == "true" || it->second == "True" || it->second == "TRUE";
	}

	float GetFloat(const std::unordered_map<std::string, std::string>& a_ini, const char* a_key, float a_default)
	{
		const auto it = a_ini.find(a_key);
		if (it == a_ini.end() || it->second.empty()) {
			return a_default;
		}
		try {
			return std::stof(it->second);
		} catch (...) {
			SKSE::log::warn("Settings: could not parse '{}' for {}, using default", it->second, a_key);
			return a_default;
		}
	}
}

void Settings::Load()
{
	constexpr auto path = "Data/SKSE/Plugins/GenderMattersDLL.ini";
	const auto ini = ParseINI(path);

	auto& t = g_data.toggles;
	t.enabled = GetBool(ini, "bEnabled", t.enabled);
	t.affectsSkills = GetBool(ini, "bAffectsSkills", t.affectsSkills);
	t.lessCarry = GetBool(ini, "bLessCarry", t.lessCarry);
	t.blockLessDmg = GetBool(ini, "bBlockLessDmg", t.blockLessDmg);
	t.getStaggeredMore = GetBool(ini, "bGetStaggeredMore", t.getStaggeredMore);
	t.sneakBetter = GetBool(ini, "bSneakBetter", t.sneakBetter);
	t.inflictLessMelee = GetBool(ini, "bInflictLessMelee", t.inflictLessMelee);
	t.takeMoreDmg = GetBool(ini, "bTakeMoreDmg", t.takeMoreDmg);
	t.rangedNerf = GetBool(ini, "bRangedNerf", t.rangedNerf);
	t.magicResistMalus = GetBool(ini, "bMagicResistMalus", t.magicResistMalus);
	t.smithingMalus = GetBool(ini, "bSmithingMalus", t.smithingMalus);
	t.hardcore = GetBool(ini, "bHardcore", t.hardcore);
	t.applyToPlayer = GetBool(ini, "bApplyToPlayer", t.applyToPlayer);
	t.applyToNPCs = GetBool(ini, "bApplyToNPCs", t.applyToNPCs);

	auto& m = g_data.magnitudes;
	m.meleeDamageDealtMult = GetFloat(ini, "fMeleeDamageDealtMult", m.meleeDamageDealtMult);
	m.meleeDamageDealtMultHardcore = GetFloat(ini, "fMeleeDamageDealtMultHardcore", m.meleeDamageDealtMultHardcore);
	m.incomingDamageMult = GetFloat(ini, "fIncomingDamageMult", m.incomingDamageMult);
	m.incomingDamageMultHardcore = GetFloat(ini, "fIncomingDamageMultHardcore", m.incomingDamageMultHardcore);
	m.incomingStaggerMult = GetFloat(ini, "fIncomingStaggerMult", m.incomingStaggerMult);
	m.incomingStaggerMultHardcore = GetFloat(ini, "fIncomingStaggerMultHardcore", m.incomingStaggerMultHardcore);
	m.blockEffectivenessMult = GetFloat(ini, "fBlockEffectivenessMult", m.blockEffectivenessMult);
	m.blockEffectivenessMultHardcore = GetFloat(ini, "fBlockEffectivenessMultHardcore", m.blockEffectivenessMultHardcore);
	m.intimidationMultHighSkill = GetFloat(ini, "fIntimidationMultHighSkill", m.intimidationMultHighSkill);
	m.intimidationMultLowSkill = GetFloat(ini, "fIntimidationMultLowSkill", m.intimidationMultLowSkill);
	m.carryWeightFlat = GetFloat(ini, "fCarryWeightFlat", m.carryWeightFlat);
	m.magicResistFlat = GetFloat(ini, "fMagicResistFlat", m.magicResistFlat);
	m.magicResistFlatHardcore = GetFloat(ini, "fMagicResistFlatHardcore", m.magicResistFlatHardcore);
	m.sneakBonusFlat = GetFloat(ini, "fSneakBonusFlat", m.sneakBonusFlat);
	m.speechBonusFlat = GetFloat(ini, "fSpeechBonusFlat", m.speechBonusFlat);
	m.alchemyBonusFlat = GetFloat(ini, "fAlchemyBonusFlat", m.alchemyBonusFlat);
	m.smithingMalusFlat = GetFloat(ini, "fSmithingMalusFlat", m.smithingMalusFlat);

	SKSE::log::info("Settings: loaded (enabled={}, hardcore={})", t.enabled, t.hardcore);
}

const Settings::Data& Settings::Get()
{
	return g_data;
}
