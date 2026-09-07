#pragma once

namespace RE
{
	class Actor;
}

// Grants/removes GM_Perk_Combat (and the flat AV bonuses that ride alongside
// it) based on an actor's sex and the applyToPlayer/applyToNPCs toggles.
// This replaces the original setup's dependency on SPID + perks_DISTR.ini -
// the DLL now does its own distribution.
namespace GenderDistributor
{
	// Call once at game start (new game / load game) for the player, and
	// once per actor as they load into a cell.
	void Sync(RE::Actor* a_actor);

	// Registers the TESObjectLoadedEvent sink that keeps NPCs in sync as
	// they stream in and out of loaded cells.
	void Install();
}
