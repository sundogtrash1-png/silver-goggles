#pragma once

// All the numbers a player can tune from GenderMattersDLL.ini, with the
// mod's original shipped values as defaults. Those defaults were pulled
// directly out of gender_matters.esp (not guessed) - see README.md for how.
namespace Settings
{
	struct Toggles
	{
		bool enabled = true;           // GM_Enabled          - master switch
		bool affectsSkills = true;     // GM_AffectsSkills    - intimidation swing
		bool lessCarry = true;         // GM_LessCarry        - carry weight malus
		bool blockLessDmg = true;      // GM_BlockLessDmg     - block effectiveness malus
		bool getStaggeredMore = true;  // GM_GetStaggeredMore - incoming stagger buff
		bool sneakBetter = true;       // GM_SneakBetter      - sneak/quiet-steps bonus
		bool inflictLessMelee = true;  // GM_InflictLessMelee - melee damage dealt malus
		bool takeMoreDmg = true;       // GM_TakeMoreDmg      - incoming damage buff (to the malus)
		bool rangedNerf = true;        // GM_RangedNerf       - disables quickdraw
		bool magicResistMalus = true;  // (was unconditional in source; now toggleable)
		bool smithingMalus = false;    // GM_Smithing         - off by default, same as source
		bool hardcore = false;         // GM_Hardcore         - harsher versions of the above

		bool applyToPlayer = true;
		bool applyToNPCs = true;
	};

	// Every multiplier below is applied to the sex the original mod targeted
	// (female). >1.0 = more/stronger, <1.0 = less/weaker.
	struct Magnitudes
	{
		float meleeDamageDealtMult = 0.85f;          // GM_Perk_Combat: kModAttackDamage (normal)
		float meleeDamageDealtMultHardcore = 0.70f;  // kModAttackDamage (hardcore)

		float incomingDamageMult = 1.15f;            // kModIncomingDamage (normal)
		float incomingDamageMultHardcore = 1.30f;    // kModIncomingDamage (hardcore)

		float incomingStaggerMult = 1.20f;           // kModIncomingStagger (normal)
		float incomingStaggerMultHardcore = 1.30f;   // kModIncomingStagger (hardcore)

		float blockEffectivenessMult = 0.85f;        // kModPercentBlocked (normal)
		float blockEffectivenessMultHardcore = 0.75f;// kModPercentBlocked (hardcore)

		// intimidation swings the other way depending on a weapon-skill check
		// in the source perk; "high" is the skilled/threatening branch.
		float intimidationMultHighSkill = 1.25f;
		float intimidationMultLowSkill = 0.75f;

		// Flat actor-value modifiers (skill points / AV units, NOT percentages).
		// carryWeight and magicResist match the exact magnitudes found in the
		// source ESP's magic effects; sneak/speech/alchemy/smithing were driven
		// by a Papyrus script in the original that isn't recoverable from static
		// data, so those four are reasonable approximations - tune freely.
		float carryWeightFlat = -50.0f;
		float magicResistFlat = -15.0f;
		float magicResistFlatHardcore = -30.0f;
		float sneakBonusFlat = 15.0f;
		float speechBonusFlat = 10.0f;
		float alchemyBonusFlat = 10.0f;
		float smithingMalusFlat = -10.0f;
	};

	struct Data
	{
		Toggles toggles;
		Magnitudes magnitudes;
	};

	// Reads Data/SKSE/Plugins/GenderMattersDLL.ini relative to the game's
	// working directory. Missing file or missing keys silently fall back to
	// the defaults above, so a bad/partial ini can never crash the load.
	void Load();

	const Data& Get();
}
