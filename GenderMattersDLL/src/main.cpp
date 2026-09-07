#include "GenderDistributor.h"
#include "PerkPatcher.h"
#include "Settings.h"

#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>

namespace
{
	void ApplyEverythingToPlayer()
	{
		if (auto* player = RE::PlayerCharacter::GetSingleton()) {
			GenderDistributor::Sync(player);
		}
	}

	void MessageHandler(SKSE::MessagingInterface::Message* a_message)
	{
		switch (a_message->type) {
		case SKSE::MessagingInterface::kDataLoaded:
			// All plugins' forms (including gender matters.esp) are
			// guaranteed loaded by this point - safe to look up perks/globals.
			Settings::Load();
			PerkPatcher::Apply();
			GenderDistributor::Install();
			break;
		case SKSE::MessagingInterface::kNewGame:
		case SKSE::MessagingInterface::kPostLoadGame:
			// Re-apply in case the ini changed between sessions, and make
			// sure the player is in sync immediately rather than waiting
			// for their next cell transition.
			Settings::Load();
			PerkPatcher::Apply();
			ApplyEverythingToPlayer();
			break;
		default:
			break;
		}
	}
}

SKSEPluginInfo(
	.Version = { 1, 0, 0 },
	.Name = "GenderMattersDLL",
	.Author = "vibecoded with Claude",
	.StructCompatibility = SKSE::StructCompatibility::Independent,
	.RuntimeCompatibility = SKSE::VersionIndependence::AddressLibrary
);

SKSEPluginLoad(const SKSE::LoadInterface* a_skse)
{
	SKSE::Init(a_skse);

	if (auto* messaging = SKSE::GetMessagingInterface()) {
		messaging->RegisterListener(MessageHandler);
	}

	return true;
}
