#pragma once

// Finds the forms shipped in gender matters.esp (by editor ID) and rewrites
// their in-memory values from Settings. This is the piece that makes the
// original mod's numbers adjustable at all - in the source mod they were
// hardcoded into the perk and only the on/off switches were exposed via
// console commands.
namespace PerkPatcher
{
	void Apply();
}
